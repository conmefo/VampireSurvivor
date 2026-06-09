#!/usr/bin/env python3
"""Extract the Vampire Survivors Forest tilemap into runtime JSON."""

from __future__ import annotations

import json
import os
import re
import shutil
import struct
import sys
from pathlib import Path


LAYER_NAMES = [
    "Floor",
    "FloorOverlay",
    "Walls",
    "FakeWalls",
    "Obstacle",
    "Decals",
    "Overlay1",
    "Shadows",
]

EXCLUDED_LAYER_RECTS = {
    # These Forest overlay fragments render as isolated pink bars in SFML and
    # are not part of the visible stone pillar bodies.
    ("FloorOverlay", 171, 174, 30, 20),
}

EXCLUDED_LAYER_REGIONS = [
    # The lower flower-square around the road junction looks noisy in this port.
    # Keep the base Floor road/grass, then replace the clutter with curated decor.
    {"layers": {"FloorOverlay", "FakeWalls", "Obstacle"}, "x1": 20, "y1": 44, "x2": 36, "y2": 50},
]

CUSTOM_DECORATIONS = [
    # Compact flower arch around the lower Forest path patch. The center and
    # bottom stay open so it reads like a garden, not a wall.
    {"x": 23, "y": 44, "rect": {"x": 409, "y": 104, "width": 32, "height": 31}},
    {"x": 24, "y": 44, "rect": {"x": 137, "y": 178, "width": 31, "height": 16}},
    {"x": 25, "y": 44, "rect": {"x": 171, "y": 103, "width": 32, "height": 32}},
    {"x": 26, "y": 44, "rect": {"x": 409, "y": 104, "width": 32, "height": 31}},
    {"x": 29, "y": 44, "rect": {"x": 409, "y": 104, "width": 32, "height": 31}},
    {"x": 30, "y": 44, "rect": {"x": 171, "y": 103, "width": 32, "height": 32}},
    {"x": 31, "y": 44, "rect": {"x": 137, "y": 178, "width": 31, "height": 16}},
    {"x": 32, "y": 44, "rect": {"x": 409, "y": 104, "width": 32, "height": 31}},
    {"x": 23, "y": 45, "rect": {"x": 137, "y": 178, "width": 31, "height": 16}},
    {"x": 32, "y": 45, "rect": {"x": 137, "y": 178, "width": 31, "height": 16}},
    {"x": 23, "y": 46, "rect": {"x": 171, "y": 103, "width": 32, "height": 32}},
    {"x": 32, "y": 46, "rect": {"x": 171, "y": 103, "width": 32, "height": 32}},
    {"x": 24, "y": 47, "rect": {"x": 409, "y": 104, "width": 32, "height": 31}},
    {"x": 31, "y": 47, "rect": {"x": 409, "y": 104, "width": 32, "height": 31}},
    {"x": 25, "y": 48, "rect": {"x": 137, "y": 178, "width": 31, "height": 16}},
    {"x": 30, "y": 48, "rect": {"x": 137, "y": 178, "width": 31, "height": 16}},
]


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


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="ignore")


def parse_ints(pattern: str, text: str) -> tuple[int, ...]:
    match = re.search(pattern, text)
    if not match:
        raise ValueError(f"Missing pattern: {pattern}")
    return tuple(int(float(value)) for value in match.groups())


def parse_map_info(prefab_text: str) -> dict[str, int]:
    width, height = parse_ints(r"m_Width:\s*(-?\d+)\s*\n\s*m_Height:\s*(-?\d+)", prefab_text)
    tile_width, tile_height = parse_ints(
        r"m_TileWidth:\s*(-?\d+)\s*\n\s*m_TileHeight:\s*(-?\d+)", prefab_text
    )
    return {
        "mapWidth": width,
        "mapHeight": height,
        "tileWidth": tile_width,
        "tileHeight": tile_height,
    }


def parse_layer_blocks(prefab_text: str) -> list[tuple[str, str]]:
    blocks: list[tuple[str, str]] = []
    cursor = 0
    while True:
        name_match = re.search(r"m_TiledName:\s*(.+)", prefab_text[cursor:])
        if not name_match:
            break

        layer_name = name_match.group(1).strip()
        name_pos = cursor + name_match.start()
        tilemap_start = prefab_text.find("--- !u!1839735485", name_pos)
        if tilemap_start == -1:
            cursor = name_pos + 1
            continue

        next_layer = prefab_text.find("m_TiledName:", tilemap_start + 1)
        block_end = len(prefab_text) if next_layer == -1 else next_layer
        blocks.append((layer_name, prefab_text[tilemap_start:block_end]))
        cursor = block_end
    return blocks


def parse_sprite_guids(tilemap_block: str) -> list[str]:
    start = tilemap_block.find("m_TileSpriteArray:")
    end = tilemap_block.find("m_TileMatrixArray:", start)
    if start == -1 or end == -1:
        return []

    array_text = tilemap_block[start:end]
    return re.findall(r"m_Data:\s*\{fileID:\s*21300000,\s*guid:\s*([0-9a-f]+),", array_text)


def parse_matrices(tilemap_block: str) -> list[dict[str, float]]:
    start = tilemap_block.find("m_TileMatrixArray:")
    end = tilemap_block.find("m_TileColorArray:", start)
    if start == -1 or end == -1:
        return []

    matrices = []
    for match in re.finditer(
        r"m_Data:\s*\n\s*e00:\s*(-?\d+(?:\.\d+)?)\s*\n\s*e01:\s*(-?\d+(?:\.\d+)?)"
        r"\s*\n\s*e02:\s*(-?\d+(?:\.\d+)?)\s*\n\s*e03:\s*(-?\d+(?:\.\d+)?)"
        r"\s*\n\s*e10:\s*(-?\d+(?:\.\d+)?)\s*\n\s*e11:\s*(-?\d+(?:\.\d+)?)"
        r"\s*\n\s*e12:\s*(-?\d+(?:\.\d+)?)\s*\n\s*e13:\s*(-?\d+(?:\.\d+)?)",
        tilemap_block[start:end],
    ):
        e00, e01, _e02, e03, e10, e11, _e12, e13 = (
            float(value) for value in match.groups()
        )
        matrices.append(
            {
                "e00": e00,
                "e01": e01,
                "e03": e03,
                "e10": e10,
                "e11": e11,
                "e13": e13,
            }
        )
    return matrices


def parse_tiles(tilemap_block: str, map_height: int) -> list[dict[str, int]]:
    start = tilemap_block.find("m_Tiles:")
    end = tilemap_block.find("m_AnimatedTiles:", start)
    if start == -1 or end == -1:
        return []

    tiles_text = tilemap_block[start:end]
    tiles: list[dict[str, int]] = []
    current: dict[str, int] | None = None
    for line in tiles_text.splitlines():
        pos_match = re.search(r"- first:\s*\{x:\s*(-?\d+),\s*y:\s*(-?\d+),\s*z:\s*(-?\d+)\}", line)
        if pos_match:
            x = int(pos_match.group(1))
            unity_y = int(pos_match.group(2))
            current = {"x": x, "y": -unity_y, "spriteIndex": 0, "matrixIndex": 0}
            if current["y"] < 0 or current["y"] >= map_height:
                current["y"] = map_height - 1 - unity_y
            continue

        sprite_match = re.search(r"m_TileSpriteIndex:\s*(\d+)", line)
        if sprite_match and current is not None:
            current["spriteIndex"] = int(sprite_match.group(1))
            continue

        matrix_match = re.search(r"m_TileMatrixIndex:\s*(\d+)", line)
        if matrix_match and current is not None:
            current["matrixIndex"] = int(matrix_match.group(1))
            continue

        flags_match = re.search(r"m_AllTileFlags:\s*\d+", line)
        if flags_match and current is not None:
            tiles.append(current)
            current = None
    return tiles


def collect_guid_paths(assets_root: Path, needed_guids: set[str]) -> dict[str, Path]:
    resolved: dict[str, Path] = {}
    if not needed_guids:
        return resolved

    guid_pattern = re.compile(r"^guid:\s*([0-9a-f]+)\s*$", re.MULTILINE)
    for meta_path in assets_root.rglob("*.meta"):
        text = read_text(meta_path)
        match = guid_pattern.search(text)
        if not match:
            continue

        guid = match.group(1)
        if guid in needed_guids:
            asset_path = Path(str(meta_path)[:-5])
            if asset_path.exists():
                resolved[guid] = asset_path
            if len(resolved) == len(needed_guids):
                break
    return resolved


def parse_sprite_rect(sprite_asset: Path, texture_height: int) -> dict[str, int]:
    text = read_text(sprite_asset)
    rect_match = re.search(
        r"textureRect:\s*\n\s*serializedVersion:\s*\d+\s*\n\s*x:\s*(-?\d+(?:\.\d+)?)"
        r"\s*\n\s*y:\s*(-?\d+(?:\.\d+)?)\s*\n\s*width:\s*(-?\d+(?:\.\d+)?)"
        r"\s*\n\s*height:\s*(-?\d+(?:\.\d+)?)",
        text,
    )
    if not rect_match:
        rect_match = re.search(
            r"m_Rect:\s*\n\s*serializedVersion:\s*\d+\s*\n\s*x:\s*(-?\d+(?:\.\d+)?)"
            r"\s*\n\s*y:\s*(-?\d+(?:\.\d+)?)\s*\n\s*width:\s*(-?\d+(?:\.\d+)?)"
            r"\s*\n\s*height:\s*(-?\d+(?:\.\d+)?)",
            text,
        )
    if not rect_match:
        raise ValueError(f"Missing sprite rectangle in {sprite_asset}")

    x, unity_y, width, height = (int(float(value)) for value in rect_match.groups())
    return {
        "x": x,
        "y": texture_height - unity_y - height,
        "width": width,
        "height": height,
    }


def parse_sprite_mesh(sprite_asset: Path, rect: dict[str, int]) -> dict:
    text = read_text(sprite_asset)
    vertex_count_match = re.search(r"m_VertexCount:\s*(\d+)", text)
    data_match = re.search(r"_typelessdata:\s*([0-9a-f]+)", text)
    index_match = re.search(r"m_IndexBuffer:\s*([0-9a-f]+)", text)
    if not vertex_count_match or not data_match or not index_match:
        width = float(rect["width"])
        height = float(rect["height"])
        return {
            "vertices": [
                {"x": 0.0, "y": 0.0, "u": rect["x"], "v": rect["y"]},
                {"x": width, "y": 0.0, "u": rect["x"] + rect["width"], "v": rect["y"]},
                {
                    "x": width,
                    "y": height,
                    "u": rect["x"] + rect["width"],
                    "v": rect["y"] + rect["height"],
                },
                {"x": 0.0, "y": height, "u": rect["x"], "v": rect["y"] + rect["height"]},
            ],
            "indices": [0, 1, 2, 0, 2, 3],
        }

    vertex_count = int(vertex_count_match.group(1))
    raw = bytes.fromhex(data_match.group(1))
    floats = struct.unpack("<" + "f" * (len(raw) // 4), raw)
    unity_vertices = [
        (floats[i * 3], floats[i * 3 + 1]) for i in range(vertex_count)
    ]
    min_x = min(x for x, _ in unity_vertices)
    max_x = max(x for x, _ in unity_vertices)
    min_y = min(y for _, y in unity_vertices)
    max_y = max(y for _, y in unity_vertices)
    width = max(max_x - min_x, 0.0001)
    height = max(max_y - min_y, 0.0001)

    vertices = []
    for x, y in unity_vertices:
        u = rect["x"] + ((x - min_x) / width) * rect["width"]
        v = rect["y"] + ((max_y - y) / height) * rect["height"]
        vertices.append({"x": x, "y": y, "u": u, "v": v})

    index_bytes = bytes.fromhex(index_match.group(1))
    indices = [
        int.from_bytes(index_bytes[i : i + 2], "little")
        for i in range(0, len(index_bytes), 2)
    ]
    return {"vertices": vertices, "indices": indices}


def build_tile_vertices(sprite_mesh: dict, transform: dict[str, float], tile_width: int) -> list[dict[str, float]]:
    scale = tile_width / 0.32
    output = []
    for vertex in sprite_mesh["vertices"]:
        x = vertex["x"]
        y = vertex["y"]
        transformed_x = transform["e00"] * x + transform["e01"] * y + transform["e03"]
        transformed_y = transform["e10"] * x + transform["e11"] * y + transform["e13"]
        output.append(
            {
                "x": transformed_x * scale,
                "y": (0.32 - transformed_y) * scale,
                "u": vertex["u"],
                "v": vertex["v"],
            }
        )
    return output


def make_quad_tile(x: int, y: int, rect: dict[str, int]) -> dict:
    width = float(rect["width"])
    height = float(rect["height"])
    return {
        "x": x,
        "y": y,
        "spriteRect": rect,
        "transform": {"e00": 1.0, "e01": 0.0, "e03": 0.0, "e10": 0.0, "e11": 1.0, "e13": 0.0},
        "vertices": [
            {"x": 0.0, "y": 0.0, "u": rect["x"], "v": rect["y"]},
            {"x": width, "y": 0.0, "u": rect["x"] + rect["width"], "v": rect["y"]},
            {
                "x": width,
                "y": height,
                "u": rect["x"] + rect["width"],
                "v": rect["y"] + rect["height"],
            },
            {"x": 0.0, "y": height, "u": rect["x"], "v": rect["y"] + rect["height"]},
        ],
        "indices": [0, 1, 2, 0, 2, 3],
    }


def parse_collision_rects(path: Path, map_height: int) -> list[dict[str, int | str]]:
    text = read_text(path)
    rects: list[dict[str, int | str]] = []
    for match in re.finditer(
        r"m_Position:\s*\{x:\s*(-?\d+(?:\.\d+)?),\s*y:\s*(-?\d+(?:\.\d+)?),\s*z:\s*-?\d+(?:\.\d+)?\}"
        r"\s*\n\s*m_Size:\s*\{x:\s*(-?\d+(?:\.\d+)?),\s*y:\s*(-?\d+(?:\.\d+)?),\s*z:\s*-?\d+(?:\.\d+)?\}",
        text,
    ):
        x, unity_y, width, height = (int(float(value)) for value in match.groups())
        rects.append(
            {
                "source": path.stem,
                "x": x,
                "y": -unity_y,
                "width": width,
                "height": height,
            }
        )
    return rects


def build_map(repo_root: Path) -> dict:
    vs_root = find_vs_root(repo_root)
    assets_root = vs_root / "ExportedProject" / "Assets"
    prefab_path = assets_root / "App" / "Art" / "Tilemaps" / "Levels" / "Forest" / "Forest.prefab"
    atlas_source = assets_root / "Texture2D" / "ForestTexturePacked.png"
    wall_path = assets_root / "MonoBehaviour" / "Forest.tmx_Walls.asset"
    player_wall_path = assets_root / "MonoBehaviour" / "Forest.tmx_PlayerWall.asset"

    prefab_text = read_text(prefab_path)
    map_info = parse_map_info(prefab_text)
    texture_height = 512

    layer_blocks = parse_layer_blocks(prefab_text)
    selected_blocks = [(name, block) for name, block in layer_blocks if name in LAYER_NAMES]
    if not selected_blocks:
        raise RuntimeError("No target tilemap layers found in Forest.prefab")

    all_sprite_guids: set[str] = set()
    layer_tiles_by_name: dict[str, tuple[list[dict[str, int]], list[str], list[dict[str, float]]]] = {}
    for name, block in selected_blocks:
        sprite_guids = parse_sprite_guids(block)
        matrices = parse_matrices(block)
        tiles = parse_tiles(block, map_info["mapHeight"])
        layer_tiles_by_name[name] = (tiles, sprite_guids, matrices)
        all_sprite_guids.update(sprite_guids)

    guid_paths = collect_guid_paths(assets_root, all_sprite_guids)
    missing_guids = sorted(all_sprite_guids - set(guid_paths))
    if missing_guids:
        raise RuntimeError(f"Missing sprite GUID assets: {', '.join(missing_guids[:20])}")

    rect_by_guid = {}
    mesh_by_guid = {}
    for guid, asset_path in guid_paths.items():
        rect = parse_sprite_rect(asset_path, texture_height)
        rect_by_guid[guid] = rect
        mesh_by_guid[guid] = parse_sprite_mesh(asset_path, rect)

    layers = []
    for name in LAYER_NAMES:
        if name not in layer_tiles_by_name:
            continue

        tiles, sprite_guids, matrices = layer_tiles_by_name[name]
        runtime_tiles = []
        for tile in tiles:
            sprite_index = tile["spriteIndex"]
            if sprite_index >= len(sprite_guids):
                raise RuntimeError(f"Layer {name} references missing sprite index {sprite_index}")
            matrix_index = tile["matrixIndex"]
            if matrix_index >= len(matrices):
                raise RuntimeError(f"Layer {name} references missing matrix index {matrix_index}")
            sprite_guid = sprite_guids[sprite_index]
            sprite_rect = rect_by_guid[sprite_guid]
            rect_key = (
                name,
                sprite_rect["x"],
                sprite_rect["y"],
                sprite_rect["width"],
                sprite_rect["height"],
            )
            if rect_key in EXCLUDED_LAYER_RECTS:
                continue
            if any(
                name in region["layers"]
                and region["x1"] <= tile["x"] <= region["x2"]
                and region["y1"] <= tile["y"] <= region["y2"]
                for region in EXCLUDED_LAYER_REGIONS
            ):
                continue

            runtime_tiles.append(
                {
                    "x": tile["x"],
                    "y": tile["y"],
                    "spriteRect": sprite_rect,
                    "transform": matrices[matrix_index],
                    "vertices": build_tile_vertices(
                        mesh_by_guid[sprite_guid], matrices[matrix_index], map_info["tileWidth"]
                    ),
                    "indices": mesh_by_guid[sprite_guid]["indices"],
                }
            )

        layers.append({"name": name, "visible": True, "tiles": runtime_tiles})

    if CUSTOM_DECORATIONS:
        decoration_tiles = [
            make_quad_tile(item["x"], item["y"], item["rect"])
            for item in CUSTOM_DECORATIONS
        ]
        insert_at = next(
            (i + 1 for i, layer in enumerate(layers) if layer["name"] == "Obstacle"),
            len(layers),
        )
        layers.insert(insert_at, {"name": "GardenDecor", "visible": True, "tiles": decoration_tiles})

    collision_rects = parse_collision_rects(wall_path, map_info["mapHeight"])
    collision_rects.extend(parse_collision_rects(player_wall_path, map_info["mapHeight"]))

    atlas_output = repo_root / "assets" / "images" / "maps" / "ForestTexturePacked.png"
    atlas_output.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(atlas_source, atlas_output)

    return {
        "atlas": "assets/images/maps/ForestTexturePacked.png",
        **map_info,
        "layers": layers,
        "collisionRects": collision_rects,
    }


def main() -> int:
    repo_root = Path(__file__).resolve().parents[1]
    output_path = repo_root / "assets" / "Data" / "maps" / "forest_map.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)

    forest_map = build_map(repo_root)
    output_path.write_text(json.dumps(forest_map, indent=2), encoding="utf-8")

    floor = next((layer for layer in forest_map["layers"] if layer["name"] == "Floor"), None)
    if forest_map["mapWidth"] != 64 or forest_map["mapHeight"] != 64:
        raise RuntimeError("Forest map dimensions are not 64x64")
    if not floor or not floor["tiles"]:
        raise RuntimeError("Generated forest map has no Floor tiles")
    if not forest_map["collisionRects"]:
        raise RuntimeError("Generated forest map has no collision rectangles")

    print(f"Wrote {output_path}")
    print(f"Copied atlas to {repo_root / forest_map['atlas']}")
    print(f"Layers: {', '.join(layer['name'] for layer in forest_map['layers'])}")
    print(f"Collision rects: {len(forest_map['collisionRects'])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
