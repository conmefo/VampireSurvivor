import os

target_dir = r"C:\Users\LENOVO\Downloads\VS_Assets\ExportedProject"
extensions = {".prefab", ".asset", ".txt", ".json", ".cs", ".yaml"}

results = []

for root, dirs, files in os.walk(target_dir):
    for f in files:
        if any(f.endswith(ext) for ext in extensions):
            path = os.path.join(root, f)
            try:
                with open(path, 'r', encoding='utf-8', errors='ignore') as file:
                    count = 0
                    for line in file:
                        if "particlesystem" in line.lower():
                            count += 1
                    if count > 0:
                        results.append((path, count))
            except Exception as e:
                pass

results.sort(key=lambda x: x[1], reverse=True)
with open(r"d:\GitHub\VampireSurvivor\search_particles_output.txt", "w") as out:
    for path, count in results[:50]:
        out.write(f"{count} matches in {path}\n")
print("Done!")
