# Release packaging

This project has two AMP ELF images. Build both projects in S32DS, then from a
clean committed tree run:

```powershell
python tools/package_release.py --config Debug_FLASH --version v0.1.0
```

The script creates a `.7z` archive under `release/` containing the committed
source tree, both ELF images, optional MAP/HEX/SREC files, debugger launch
files, and `MANIFEST.txt` with the source commit and SHA-256 hashes. The
versioned filename means multiple milestone/release archives can coexist in
`release/`; an existing archive with the same name must be reviewed before it
is replaced. It does not invoke S32DS or modify generated RTD files. It requires `7z.exe` on PATH
or the `SEVENZIP_EXE` environment variable.

The maintainer reviews the archive, commits the release artifact (and ideally
creates a Git tag), then pushes it. Recipients can use the prebuilt firmware
without running this script; they only need the documented S32DS/SDK toolchain
when rebuilding from source.
