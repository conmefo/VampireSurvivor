import os

target_dir = r"C:\Users\LENOVO\Downloads\VS_Assets"
extensions = {".prefab", ".asset", ".txt", ".json", ".cs", ".yaml"}

results = []

for root, dirs, files in os.walk(target_dir):
    for f in files:
        if any(f.endswith(ext) for ext in extensions):
            path = os.path.join(root, f)
            try:
                with open(path, 'r', encoding='utf-8') as file:
                    count = 0
                    for line in file:
                        if "particlesystem" in line.lower():
                            count += 1
                    if count > 0:
                        results.append((path, count))
            except Exception:
                pass

results.sort(key=lambda x: x[1], reverse=True)
for path, count in results[:20]:
    print(f"{count} matches in {path}")
