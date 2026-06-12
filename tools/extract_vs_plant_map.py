#!/usr/bin/env python3
"""Extract the Vampire Survivors Dairy Plant tilemap into runtime JSON."""

from __future__ import annotations

import json
import shutil
import sys
from pathlib import Path

sys.dont_write_bytecode = True

import extract_vs_forest_map as tilemap


LAYER_NAMES = [
    "Floor",
    "FloorOverlay",
    "Walls",
    "FakeWalls",
    "PlayerWall",
    "Obstacle",
    "Decals",
    "Overlay1",
    "Shadows",
]


def build_map(repo_root: Path) -> dict:
    vs_root = tilemap.find_vs_root(repo_root)
    assets_root = vs_root / "ExportedProject" / "Assets"
    prefab_path = (
        assets_root / "App" / "Art" / "Tilemaps" / "Levels" / "Plant" / "Plant2.prefab"
    )
    atlas_source = assets_root / "Texture2D" / "PlantTexturePacked.png"
    wall_path = assets_root / "MonoBehaviour" / "Plant2.tmx_Walls.asset"
    player_wall_path = assets_root / "MonoBehaviour" / "Plant2.tmx_PlayerWall.asset"

    prefab_text = tilemap.read_text(prefab_path)
    map_info = tilemap.parse_map_info(prefab_text)
    texture_height = 512

    layer_blocks = tilemap.parse_layer_blocks(prefab_text)
    selected_blocks = [(name, block) for name, block in layer_blocks if name in LAYER_NAMES]
    if not selected_blocks:
        raise RuntimeError("No target tilemap layers found in Plant2.prefab")

    all_sprite_guids: set[str] = set()
    layer_tiles_by_name = {}
    for name, block in selected_blocks:
        sprite_guids = tilemap.parse_sprite_guids(block)
        matrices = tilemap.parse_matrices(block)
        tiles = tilemap.parse_tiles(block, map_info["mapHeight"])
        layer_tiles_by_name[name] = (tiles, sprite_guids, matrices)
        all_sprite_guids.update(sprite_guids)

    guid_paths = tilemap.collect_guid_paths(assets_root, all_sprite_guids)
    missing_guids = sorted(all_sprite_guids - set(guid_paths))
    if missing_guids:
        raise RuntimeError(f"Missing sprite GUID assets: {', '.join(missing_guids[:20])}")

    rect_by_guid = {}
    mesh_by_guid = {}
    for guid, asset_path in guid_paths.items():
        rect = tilemap.parse_sprite_rect(asset_path, texture_height)
        rect_by_guid[guid] = rect
        mesh_by_guid[guid] = tilemap.parse_sprite_mesh(asset_path, rect)

    layers = []
    for name in LAYER_NAMES:
        if name not in layer_tiles_by_name:
            continue

        tiles, sprite_guids, matrices = layer_tiles_by_name[name]
        runtime_tiles = []
        for source_tile in tiles:
            sprite_index = source_tile["spriteIndex"]
            if sprite_index >= len(sprite_guids):
                raise RuntimeError(f"Layer {name} references missing sprite index {sprite_index}")
            matrix_index = source_tile["matrixIndex"]
            if matrix_index >= len(matrices):
                raise RuntimeError(f"Layer {name} references missing matrix index {matrix_index}")

            sprite_guid = sprite_guids[sprite_index]
            sprite_rect = rect_by_guid[sprite_guid]
            runtime_tiles.append(
                {
                    "x": source_tile["x"],
                    "y": source_tile["y"],
                    "spriteRect": sprite_rect,
                    "transform": matrices[matrix_index],
                    "vertices": tilemap.build_tile_vertices(
                        mesh_by_guid[sprite_guid],
                        matrices[matrix_index],
                        map_info["tileWidth"],
                    ),
                    "indices": mesh_by_guid[sprite_guid]["indices"],
                }
            )

        layers.append({"name": name, "visible": True, "tiles": runtime_tiles})

    collision_rects = tilemap.parse_collision_rects(wall_path, map_info["mapHeight"])
    collision_rects.extend(tilemap.parse_collision_rects(player_wall_path, map_info["mapHeight"]))

    atlas_output = repo_root / "assets" / "images" / "maps" / "PlantTexturePacked.png"
    atlas_output.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(atlas_source, atlas_output)

    return {
        "atlas": "assets/images/maps/PlantTexturePacked.png",
        **map_info,
        "layers": layers,
        "collisionRects": collision_rects,
    }


def main() -> int:
    repo_root = Path(__file__).resolve().parents[1]
    output_path = repo_root / "assets" / "Data" / "maps" / "plant_map.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)

    plant_map = build_map(repo_root)
    output_path.write_text(json.dumps(plant_map, indent=2), encoding="utf-8")

    floor = next((layer for layer in plant_map["layers"] if layer["name"] == "Floor"), None)
    if plant_map["mapWidth"] != 64 or plant_map["mapHeight"] != 64:
        raise RuntimeError("Plant map dimensions are not 64x64")
    if not floor or not floor["tiles"]:
        raise RuntimeError("Generated plant map has no Floor tiles")
    if not plant_map["collisionRects"]:
        raise RuntimeError("Generated plant map has no collision rectangles")

    print(f"Wrote {output_path}")
    print(f"Copied atlas to {repo_root / plant_map['atlas']}")
    print(f"Layers: {', '.join(layer['name'] for layer in plant_map['layers'])}")
    print(f"Collision rects: {len(plant_map['collisionRects'])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
