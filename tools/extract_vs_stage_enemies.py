#!/usr/bin/env python3
"""Extract enemy definitions for stage 2 and stage 3."""

from __future__ import annotations

import json
import shutil
import sys
from pathlib import Path

sys.dont_write_bytecode = True

import extract_vs_forest_enemies as enemy_tools


STAGE_PLACES = {"LIBRARY", "WAREHOUSE"}
EXCLUDED_ENEMY_KEYS = {"FLOWER"}


def stage_enemy_keys(enemy_data: dict) -> list[str]:
    keys: list[str] = []
    for enemy_key, variants in enemy_data.items():
        if enemy_key in EXCLUDED_ENEMY_KEYS or not isinstance(variants, list):
            continue
        if not variants:
            continue

        places = set(variants[0].get("bPlaces", []) or [])
        if places & STAGE_PLACES:
            keys.append(enemy_key)

    return keys


def runtime_defaults(frame_rects: list[dict[str, int]]) -> dict[str, float | int]:
    if not frame_rects:
        return {"mass": 1, "collisionRadius": 14, "spriteScale": 1}

    max_frame_size = max(max(rect["width"], rect["height"]) for rect in frame_rects)
    collision_radius = max(8, min(32, int(max_frame_size * 0.45)))
    if collision_radius >= 24:
        mass = 2.0
    elif collision_radius >= 16:
        mass = 1.3
    else:
        mass = 1.0

    return {
        "mass": mass,
        "collisionRadius": collision_radius,
        "spriteScale": 1,
    }


def build_enemy_data(repo_root: Path) -> dict:
    vs_root = enemy_tools.find_vs_root(repo_root)
    assets_root = vs_root / "ExportedProject" / "Assets"
    spritesheet_root = assets_root / "Resources" / "spritesheets"
    enemy_data_path = assets_root / "TextAsset" / "ENEMY_DATA.json"
    enemy_data = json.loads(enemy_tools.read_text(enemy_data_path))
    texture_guid_index = enemy_tools.build_texture_guid_index(assets_root)

    output_texture_dir = repo_root / "assets" / "images" / "enemies"
    output_texture_dir.mkdir(parents=True, exist_ok=True)

    atlas: dict[str, dict[str, int | str]] = {}
    animations: dict[str, dict[str, object]] = {}
    stats: dict[str, dict[str, object]] = {}
    copied_textures: dict[Path, str] = {}

    for enemy_key in stage_enemy_keys(enemy_data):
        enemy = enemy_data[enemy_key][0]
        first_frames = enemy.get("frameNames") or []
        if not first_frames:
            continue

        frames = enemy_tools.numbered_frames(
            first_frames[0],
            int(enemy.get("idleFrameCount", 1)),
            spritesheet_root,
        )
        animation_frames: list[str] = []
        frame_rects: list[dict[str, int]] = []

        for frame in frames:
            sprite_asset = spritesheet_root / f"{frame}.asset"
            if not sprite_asset.exists():
                continue

            texture_path = enemy_tools.guid_asset_path(
                texture_guid_index,
                enemy_tools.texture_guid(sprite_asset),
            )
            texture_width, texture_height = enemy_tools.texture_size(texture_path)
            rect = enemy_tools.parse_sprite_rect(sprite_asset, texture_height)
            frame_rects.append(rect)

            if texture_path not in copied_textures:
                output_name = f"vs_{texture_path.name}"
                shutil.copyfile(texture_path, output_texture_dir / output_name)
                copied_textures[texture_path] = f"assets/images/enemies/{output_name}"

            asset_id = f"{enemy_key.lower()}_{frame.lower()}"
            atlas[asset_id] = {
                "texture": copied_textures[texture_path],
                "x": rect["x"],
                "y": rect["y"],
                "width": rect["width"],
                "height": rect["height"],
                "sourceWidth": texture_width,
                "sourceHeight": texture_height,
            }
            animation_frames.append(asset_id)

        if not animation_frames:
            continue

        animations[enemy_key] = {
            "name": enemy.get("bName", enemy_key),
            "textureName": enemy.get("textureName", ""),
            "states": {
                "idle": {
                    "frameDuration": 0.16,
                    "isLooping": True,
                    "frames": animation_frames,
                }
            },
        }
        stats[enemy_key] = {
            "level": enemy.get("level", 1),
            "maxHp": enemy.get("maxHp", 1),
            "speed": enemy.get("speed", 100),
            "power": enemy.get("power", 1),
            "knockback": enemy.get("knockback", 1),
            "xp": enemy.get("xp", 1),
            **runtime_defaults(frame_rects),
            "places": enemy.get("bPlaces", []),
        }

    return {
        "atlas": atlas,
        "animations": animations,
        "stats": stats,
    }


def main() -> int:
    repo_root = Path(__file__).resolve().parents[1]
    output_path = repo_root / "assets" / "Data" / "enemies" / "stage_enemies.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)

    data = build_enemy_data(repo_root)
    output_path.write_text(json.dumps(data, indent=2), encoding="utf-8")

    print(f"Wrote {output_path}")
    print("Enemies:", ", ".join(data["animations"].keys()))
    print("Frames:", sum(len(enemy["states"]["idle"]["frames"]) for enemy in data["animations"].values()))
    print("Textures:", ", ".join(sorted({entry["texture"] for entry in data["atlas"].values()})))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
