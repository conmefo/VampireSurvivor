#!/usr/bin/env python3
"""Extract a small Forest enemy set from the Vampire Survivors assets."""

from __future__ import annotations

import json
import re
import shutil
from pathlib import Path


ENEMY_KEYS = ["BAT1", "SKELETON"]


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")


def find_vs_root(repo_root: Path) -> Path:
    candidates = [
        repo_root / "vs_assets" / "VS_Assets",
        repo_root / "VS_Assets" / "VS_Assets",
        repo_root / "VS_Assets",
        repo_root / "vs_assets",
    ]
    for candidate in candidates:
        if (candidate / "ExportedProject" / "Assets").exists():
            return candidate
    raise FileNotFoundError("Could not find VS_Assets/ExportedProject/Assets")


def parse_sprite_rect(sprite_asset: Path, texture_height: int) -> dict[str, int]:
    text = read_text(sprite_asset)
    match = re.search(
        r"textureRect:\s*\n\s*serializedVersion:\s*\d+\s*\n\s*x:\s*(-?\d+(?:\.\d+)?)"
        r"\s*\n\s*y:\s*(-?\d+(?:\.\d+)?)\s*\n\s*width:\s*(-?\d+(?:\.\d+)?)"
        r"\s*\n\s*height:\s*(-?\d+(?:\.\d+)?)",
        text,
    )
    if not match:
        raise ValueError(f"Missing sprite rectangle in {sprite_asset}")

    x, unity_y, width, height = (int(float(value)) for value in match.groups())
    return {
        "x": x,
        "y": texture_height - unity_y - height,
        "width": width,
        "height": height,
    }


def texture_guid(sprite_asset: Path) -> str:
    text = read_text(sprite_asset)
    match = re.search(r"texture:\s*\{fileID:\s*2800000,\s*guid:\s*([0-9a-f]+),", text)
    if not match:
        raise ValueError(f"Missing texture GUID in {sprite_asset}")
    return match.group(1)


def guid_asset_path(assets_root: Path, guid: str) -> Path:
    for meta_path in assets_root.rglob("*.meta"):
        if f"guid: {guid}" not in read_text(meta_path):
            continue

        asset_path = Path(str(meta_path)[:-5])
        if asset_path.exists():
            return asset_path

    raise FileNotFoundError(f"Could not resolve GUID {guid}")


def frame_stem(frame_name: str) -> str:
    return Path(frame_name).stem


def numbered_frames(first_frame: str, idle_frame_count: int, spritesheet_root: Path) -> list[str]:
    stem = frame_stem(first_frame)
    base_match = re.match(r"(.+)_\d+$", stem)
    if base_match:
        base = base_match.group(1)
        idle_frames = [
            f"{base}_i{index:02}"
            for index in range(1, max(idle_frame_count, 1) + 1)
        ]
        if all((spritesheet_root / f"{frame}.asset").exists() for frame in idle_frames):
            return idle_frames

    match = re.match(r"(.+_)\d+$", stem)
    if not match:
        return [stem]

    prefix = match.group(1)
    return [f"{prefix}{index}" for index in range(max(idle_frame_count, 1))]


def texture_size(path: Path) -> tuple[int, int]:
    # PNG IHDR stores width/height as big-endian uint32 at bytes 16..24.
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError(f"Not a PNG texture: {path}")
    return int.from_bytes(data[16:20], "big"), int.from_bytes(data[20:24], "big")


def build_enemy_data(repo_root: Path) -> dict:
    vs_root = find_vs_root(repo_root)
    assets_root = vs_root / "ExportedProject" / "Assets"
    spritesheet_root = assets_root / "Resources" / "spritesheets"
    enemy_data_path = assets_root / "TextAsset" / "ENEMY_DATA.json"
    enemy_data = json.loads(read_text(enemy_data_path))

    output_texture_dir = repo_root / "assets" / "images" / "enemies"
    output_texture_dir.mkdir(parents=True, exist_ok=True)

    atlas: dict[str, dict[str, int | str]] = {}
    animations: dict[str, dict[str, object]] = {}
    stats: dict[str, dict[str, object]] = {}
    copied_textures: dict[Path, str] = {}

    for enemy_key in ENEMY_KEYS:
        variants = enemy_data.get(enemy_key)
        if not variants:
            raise KeyError(f"Missing enemy data for {enemy_key}")

        enemy = variants[0]
        first_frames = enemy.get("frameNames") or []
        if not first_frames:
            raise ValueError(f"Enemy {enemy_key} has no frameNames")

        frames = numbered_frames(
            first_frames[0],
            int(enemy.get("idleFrameCount", 1)),
            spritesheet_root,
        )
        animation_frames: list[str] = []
        texture_name_for_enemy = ""

        for frame in frames:
            sprite_asset = spritesheet_root / f"{frame}.asset"
            if not sprite_asset.exists():
                continue

            texture_path = guid_asset_path(assets_root, texture_guid(sprite_asset))
            texture_width, texture_height = texture_size(texture_path)
            rect = parse_sprite_rect(sprite_asset, texture_height)

            if texture_path not in copied_textures:
                output_name = f"vs_{texture_path.name}"
                shutil.copyfile(texture_path, output_texture_dir / output_name)
                copied_textures[texture_path] = f"assets/images/enemies/{output_name}"

            texture_name_for_enemy = copied_textures[texture_path]
            asset_id = f"{enemy_key.lower()}_{frame.lower()}"
            atlas[asset_id] = {
                "texture": texture_name_for_enemy,
                "x": rect["x"],
                "y": rect["y"],
                "width": rect["width"],
                "height": rect["height"],
                "sourceWidth": texture_width,
                "sourceHeight": texture_height,
            }
            animation_frames.append(asset_id)

        if not animation_frames:
            raise RuntimeError(f"No sprite assets resolved for {enemy_key}")

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
            "places": enemy.get("bPlaces", []),
        }

    return {
        "atlas": atlas,
        "animations": animations,
        "stats": stats,
    }


def main() -> int:
    repo_root = Path(__file__).resolve().parents[1]
    output_path = repo_root / "assets" / "Data" / "enemies" / "forest_enemies.json"
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
