# S32K358 FreeRTOS peripheral-test software

This repository contains the S32 Design Studio projects and engineering records for a FreeRTOS-based test application targeting the FRDM-A-S32K358 board. The physical board is the behavioral reference used to validate the corresponding QEMU and future ASIC models.

## Repository layout

- `FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_0/`: lockstep application-core image (`CM7_0` with `CM7_1` checker).
- `FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_2/`: independent `CM7_2` image.
- `docs/`: project context, roadmap, decisions, engineering rules, references, and test evidence.
- `AGENTS.md`: mandatory context for future Codex agents.

The `.mex` files, S32DS project metadata, launch configurations, startup/linker files, generated configuration, and attached RTD/FreeMASTER sources are intentionally versioned. Managed-build output and Eclipse workspace metadata are intentionally excluded.

## Frozen tool baseline

The initial generated project currently identifies the following toolchain. Confirm these versions on every development host before accepting a build as reproducible:

- S32 Design Studio for S32 Platform: `3.6.5`
- S32K3 PlatformSDK / Real-Time Drivers: `7.0.0`
- NXP GCC for Arm bare metal: `10.2`
- FreeMASTER SDK: `1.5.0`
- Target: `S32K358`, package `289BGA`

The two current `.mex` files report different internal Clocks-tool revisions. This is an open baseline issue documented in `docs/PROJECT_CONTEXT.md` and must be normalized through a controlled **Update Code and Build** cycle.

## Recreating the workspace after cloning

1. Install S32 Design Studio and the exact SDK versions listed above.
2. Create or select a new, empty S32DS workspace. Do not reuse or copy the `.metadata` directory from another computer.
3. Choose **File > Import > General > Existing Projects into Workspace**.
4. Select the cloned `FreeRTOS_Test_SW` directory as the root and import both core projects without copying them to another location.
5. Confirm that both projects are open and that their SDK references resolve without errors.
6. For each project, select it in Project Explorer and run **Update Code and Build**. A plain Build is not sufficient for a project containing a `.mex` configuration.
7. Verify that both projects build without errors and that the group FLASH launch configurations under the `M7_0_0` project resolve both ELF files.

Do not edit files under `generate/` or `board/` manually. Change the corresponding `.mex` configuration and regenerate them. Review generated-code changes through Git before accepting them.

## Hardware evidence

A successful build is not a passed test. Each roadmap phase must be flashed and exercised on the FRDM-A-S32K358, with results recorded in `docs/TEST_LOG.md` before comparison with QEMU.

