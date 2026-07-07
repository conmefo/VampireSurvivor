import os
import shutil
import re

# ==========================================
# CONFIGURATION
# ==========================================

# 1. Add as many GUIDs as you want inside this list
TARGET_GUIDS = [
    "ee7c81394b948f0458ee89a037256849", # MagicMissileProjectile / HolyWand
    "b08e89d4ef8faea49a43ab32b908482c", # KnifeProjectile
    "79c21d7aecc37804ea31e4226ccd7434", # AxeProjectile
]

# 2. Change this to the folder where your extracted assets (.png and .meta) are located
RESOURCE_FOLDER = r"C:\Users\Admin\Downloads\VS_Assets"

# 3. The folder where the found sprites will be copied
RESULT_FOLDER = r"./result_sprites"

# ==========================================
# SCRIPT LOGIC
# ==========================================

def main():
    if not os.path.exists(RESULT_FOLDER):
        os.makedirs(RESULT_FOLDER)

    target_set = set(g.strip() for g in TARGET_GUIDS if g.strip())

    print(f"Scanning '{RESOURCE_FOLDER}' to build GUID database. This might take a few seconds...")
    if not os.path.exists(RESOURCE_FOLDER):
        print(f"\n[ERROR] The resource folder '{RESOURCE_FOLDER}' does not exist.")
        return

    # Build a dictionary mapping GUID -> Actual File Path (ignoring the .meta extension)
    guid_to_path = {}
    for root, dirs, files in os.walk(RESOURCE_FOLDER):
        for file in files:
            if file.endswith(".meta"):
                meta_path = os.path.join(root, file)
                try:
                    with open(meta_path, 'r', encoding='utf-8') as f:
                        for line in f:
                            if line.startswith("guid:"):
                                guid = line.split("guid:")[1].strip()
                                actual_filename = file[:-5]
                                guid_to_path[guid] = os.path.join(root, actual_filename)
                                break
                except Exception:
                    pass

    print(f"Built database of {len(guid_to_path)} items.\n")
    print(f"Extracting target sprites...")
    print("="*40)

    for target_guid in target_set:
        if target_guid not in guid_to_path:
            print(f"[MISSING] Target GUID {target_guid} not found in any .meta file.")
            continue
        
        target_path = guid_to_path[target_guid]
        
        # If it's directly a PNG, just copy it
        if target_path.lower().endswith(".png") or target_path.lower().endswith(".jpg"):
            dest_path = os.path.join(RESULT_FOLDER, f"{target_guid}_{os.path.basename(target_path)}")
            shutil.copy2(target_path, dest_path)
            print(f"[DIRECT PNG] {target_guid} -> Copied {os.path.basename(target_path)}")
            continue
            
        # If it's a Sprite Asset (like .asset or .sprite), it means it's packed in a spritesheet
        if target_path.lower().endswith(".asset") or target_path.lower().endswith(".sprite"):
            print(f"[SPRITESHEET REF] {target_guid} -> Found {os.path.basename(target_path)}. Tracing to master texture...")
            
            try:
                with open(target_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                    # 1. Find the master texture GUID
                    tex_match = re.search(r'texture: {.*guid: ([a-f0-9]{32})', content)
                    if not tex_match:
                        print(f"   -> Error: Could not find texture reference inside {os.path.basename(target_path)}")
                        continue
                    
                    master_guid = tex_match.group(1)
                    
                    if master_guid not in guid_to_path:
                        print(f"   -> Error: Found master texture GUID {master_guid}, but it's missing from resources!")
                        continue
                        
                    master_path = guid_to_path[master_guid]
                    
                    # 2. Find the rectangle coordinates
                    rect_match = re.search(r'textureRect:\s*\n\s*serializedVersion:.*\n\s*x: (\d+(?:\.\d+)?)\n\s*y: (\d+(?:\.\d+)?)\n\s*width: (\d+(?:\.\d+)?)\n\s*height: (\d+(?:\.\d+)?)', content)
                    
                    # Copy the massive spritesheet
                    master_dest = os.path.join(RESULT_FOLDER, f"spritesheet_{os.path.basename(master_path)}")
                    if not os.path.exists(master_dest):
                        shutil.copy2(master_path, master_dest)
                        
                    print(f"   -> [SUCCESS] Extracted spritesheet: {os.path.basename(master_path)}")
                    
                    # Create an info text file so the user knows how to slice it!
                    info_dest = os.path.join(RESULT_FOLDER, f"{target_guid}_{os.path.basename(target_path)}_slice_info.txt")
                    with open(info_dest, 'w', encoding='utf-8') as info_f:
                        info_f.write(f"Target Weapon/Projectile GUID: {target_guid}\n")
                        info_f.write(f"Sprite Name: {os.path.basename(target_path)}\n")
                        info_f.write(f"Uses Spritesheet: {os.path.basename(master_path)}\n\n")
                        
                        if rect_match:
                            x, y, w, h = rect_match.groups()
                            info_f.write(f"--- SLICE COORDINATES ---\n")
                            info_f.write(f"X: {x}\n")
                            info_f.write(f"Y: {y}\n")
                            info_f.write(f"Width: {w}\n")
                            info_f.write(f"Height: {h}\n")
                            print(f"   -> Generated Slice Info: X={x}, Y={y}, W={w}, H={h}")
                        else:
                            info_f.write("Could not parse textureRect from the asset file.\n")

            except Exception as e:
                print(f"   -> Error parsing asset {os.path.basename(target_path)}: {e}")

    print("\n" + "="*40)
    print("Double-Jump Extraction Complete! Check your './result_sprites' folder.")

if __name__ == "__main__":
    main()
