# Release packaging

This project has two AMP ELF images. Build both projects in S32DS, then from a
clean committed tree run:

```powershell
python tools/package_release.py --config Debug_FLASH --version v0.2.0
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

## v0.2.0 milestone scope

This milestone introduces the debugger-selected STM test software on the
existing AMP images. It includes functional checks `STM-001` through `STM-004`
on CM7_0/STM0_CH3 and CM7_2/STM2_CH3, while preserving channels 0-2 for the
periodic TaskA/TaskB releases. The recorded hardware checks are in
`docs/TEST_LOG.md` (`RUN-20261025-018` through `RUN-20261025-020`).

This is an initial STM test-software release, not a claim that the full STM
validation phase or hardware/QEMU parity is complete. Remaining functional
coverage and deterministic canonical result matching on QEMU are open; physical
jitter measurement is not a release requirement. Create the archive only after
the source and documentation are committed and both `Debug_FLASH` ELFs have
been rebuilt from that revision. Review the generated manifest and archive
before committing and pushing the release artifact.
