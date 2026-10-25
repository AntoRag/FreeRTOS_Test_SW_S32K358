# Releases and packaging

Releases are versioned `.7z` packages retained under `release/`. The root [README](../README.md#releases) lists what each release introduces. Package labels identify repository artifacts; they do not imply that a Git tag or a GitHub Release object exists.

## Published package records

| Version | Configuration | Manifest source commit | Archive commit | Hardware evidence for the included functionality |
|---|---|---|---|---|
| `v0.2.0` | `Debug_FLASH` | `8b7370d49a6c6208860e0690e9f9a03197b3894c` | `c868d93` | STM-001..004 functional checks on both cores: RUN-20261025-018/019/020. |
| `v0.1.0` | `Debug_FLASH` | `ed99cc3e6a0997b2fcf33009a25e666bb6e91eba` | `030d0ee` | Common-source dual-core task/runtime and protocol-V3 READY checks: RUN-20261025-016/017. |

The source commit identifies the tree copied into the package. The archive commit adds that completed package to Git afterward. These are different identities.

Documentation inside each published package remains a snapshot of its source commit. Corrections in the current checkout do not rewrite historical archives.

| Package | SHA-256 of archive |
|---|---|
| [v0.2.0](../release/S32K358_FreeRTOS_v0.2.0_Debug_FLASH.7z) | `D72E1B0E84F4C2061142C1EAF2DF8CE992FD4849178B29C52F6677B24898EC7F` |
| [v0.1.0](../release/S32K358_FreeRTOS_v0.1.0_Debug_FLASH.7z) | `6E334FAAC1311B946BDE2E83E0B3861D734F77B8656420F53A134609D00E01A3` |

The manifest in each archive records SHA-256 values for both ELF/map pairs and the three debugger launch files. The packages preserve later prebuilt firmware identities; they do not retroactively identify the exact binaries used in the earlier screenshot-only board runs. Full build-console logs and a board replay tied to the package hashes remain separate evidence.

## Package contents and scope

Each archive has a versioned top-level folder containing:

- `source/`: the tracked source tree exported from the manifest's source commit;
- `firmware/M7_0_0/` and `firmware/M7_0_2/`: prebuilt ELF images and available MAP/HEX/SREC products;
- `debug/`: the FLASH Launch Group and its two child launch files;
- `MANIFEST.txt`: release label, configuration, source commit, and collected artifact hashes.

`v0.1.0` establishes the common AMP runtime milestone. `v0.2.0` adds the first debugger-selected STM tests on both cores; see the [STM component guide](test_sw/STM.md). Their functional board results are recorded in [TEST_LOG.md](TEST_LOG.md). Neither release claims standalone cold-boot qualification, complete timer coverage, or QEMU parity. Physical jitter measurement is not a release requirement (D-027).

The host-local PEmicro macro workaround is required for the recorded plug-in version and is not installed by unpacking a release. The `v0.1.0` source/generated configuration disables STM debug freeze on both instances; `v0.2.0` enables it on both, matching the current baseline. See [the build/debug guide](BUILD_AND_DEBUG.md) and D-021/D-023 before debugging on another workstation or reproducing an older release.

## Creating the next package

The maintainer runs the Python script manually. Recipients can use the prebuilt firmware without running it. GitHub-runner automation is future work.

1. Record the milestone scope and known gaps; commit the intended source and documentation revision.
2. Build both core projects in S32DS using the intended configuration, validating/regenerating `.mex` output as required. Preserve the build logs and ensure the prebuilt products correspond to the intended sources.
3. From the repository root and a clean committed working tree, run the following, replacing `NEW_VERSION` with an unused release label:

```powershell
python tools/package_release.py --config Debug_FLASH --version NEW_VERSION
```

4. Inspect the archive, source commit, firmware products, and manifest hashes. Keep existing published packages unchanged.
5. Commit the new archive, update the release records/README table, and push. A Git tag is optional; record its target if one is created.

The host needs Python 3, Git, `tar`, and 7-Zip. The script discovers `7z`/`7zz`, `SEVENZIP_EXE`, or the standard Windows 7-Zip installation paths. It never invokes S32DS or runs a board test.

## Current packager limitations

- It checks the tracked/untracked working tree, but ignored build outputs can still be stale. It records a source commit and binary hashes without proving that those binaries were compiled from that commit.
- It exports all tracked files with `git archive HEAD`, including older release archives inside `source/release/`. That is current behavior and can increase package size as history grows.
- It uses `7z a`; it does not refuse an existing output filename. Always choose a new version instead of updating a published archive in place.
- It does not capture build-console logs, tool discovery, hardware results, or a QEMU run automatically. Those belong in the evidence process.
