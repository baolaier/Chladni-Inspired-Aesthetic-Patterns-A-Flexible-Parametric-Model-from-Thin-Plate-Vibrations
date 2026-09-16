import subprocess
from pathlib import Path

project_dir = Path(__file__).resolve().parent.parent
executable = project_dir / "Release" / "OpenGl.exe"

type_id = 0
texture_path = project_dir / "Texture" / "1#5000.png"
save_dir = project_dir / "output"

# example parameters for the executable
params = [
    [11.187, 15.0, 3.0, 4.0, 16, 16, 0.434, 0.434],
    [11.187, 15.0, 3.0, 4.0, 16, 26, 0.500, 0.500],
    [11.187, 15.0, 3.0, 4.0, 16, 30, 0.600, 0.600],
]

if not executable.is_file():
    raise FileNotFoundError(f"Executable not found: {executable}")
if not texture_path.is_file():
    raise FileNotFoundError(f"Texture not found: {texture_path}")

input_data = "".join(" ".join(map(str, p)) + "\n" for p in params)
subprocess.run(
    [
        str(executable),
        "--cli",
        str(type_id),
        str(texture_path),
        str(save_dir),
    ],
    input=input_data,
    text=True,
    cwd=project_dir,
    check=True,
)

print("All done.")
