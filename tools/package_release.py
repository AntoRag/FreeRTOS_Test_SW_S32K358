#!/usr/bin/env python3
"""Package prebuilt S32K358 AMP outputs into a manifested 7z archive."""
from __future__ import annotations
import argparse, hashlib, os, shutil, subprocess, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PROJECTS = {
    "M7_0_0": ROOT / "FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_0",
    "M7_0_2": ROOT / "FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_2",
}

def command(*args: str, cwd: Path = ROOT) -> str:
    return subprocess.run(args, cwd=cwd, check=True, text=True,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout.strip()

def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()

def seven_zip() -> str:
    candidates = [shutil.which("7z"), shutil.which("7zz"), os.environ.get("SEVENZIP_EXE"),
                  r"C:\Program Files\7-Zip\7z.exe", r"C:\Program Files (x86)\7-Zip\7z.exe"]
    for item in candidates:
        if item and Path(item).is_file():
            return str(item)
    raise RuntimeError("7z.exe not found; install 7-Zip or set SEVENZIP_EXE")

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", default="Debug_FLASH")
    parser.add_argument("--version", required=True, help="release label, e.g. v0.1.0 or H1-20261025")
    parser.add_argument("--output-dir", type=Path, default=ROOT / "release")
    args = parser.parse_args()
    if command("git", "status", "--porcelain"):
        raise RuntimeError("working tree is not clean; commit the source before packaging")
    commit = command("git", "rev-parse", "HEAD")
    archive_name = f"S32K358_FreeRTOS_{args.version}_{args.config}.7z"
    args.output_dir.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="s32k358-release-") as temp:
        root = Path(temp) / archive_name[:-3]
        source = root / "source"
        source.mkdir(parents=True)
        tar = Path(temp) / "source.tar"
        with tar.open("wb") as stream:
            subprocess.run(["git", "archive", "--format=tar", "HEAD"], cwd=ROOT,
                           check=True, stdout=stream)
        subprocess.run(["tar", "-xf", str(tar), "-C", str(source)], check=True)
        firmware = root / "firmware"
        collected = []
        for core, project in PROJECTS.items():
            build = project / args.config
            elfs = list(build.glob("*.elf"))
            if len(elfs) != 1:
                raise RuntimeError(f"expected one ELF in {build}, found {len(elfs)}")
            target = firmware / core
            target.mkdir(parents=True)
            for item in [*elfs, *build.glob("*.map"), *build.glob("*.hex"), *build.glob("*.srec")]:
                out = target / item.name
                shutil.copy2(item, out)
                collected.append(out)
        debug = root / "debug"
        debug.mkdir()
        for launch in ROOT.glob("FreeRTOS_Test_SW/*/Project_Settings/Debugger/*.launch"):
            out = debug / launch.name
            shutil.copy2(launch, out)
            collected.append(out)
        manifest = root / "MANIFEST.txt"
        lines = [f"release={args.version}", f"configuration={args.config}",
                 f"git_commit={commit}", "build_outputs=prebuilt by S32DS", ""]
        lines += [f"{p.relative_to(root)}  {digest(p)}" for p in sorted(collected)]
        manifest.write_text("\n".join(lines) + "\n", encoding="utf-8")
        output = args.output_dir / archive_name
        subprocess.run([seven_zip(), "a", "-t7z", str(output), root.name],
                       cwd=root.parent, check=True)
    print(f"Created {output}\nCommit {commit}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
