# Project Context

## Mission

Build a professional FreeRTOS-based validation application for the NXP S32K358 that:

- teaches the user how to configure, build, flash, and debug with S32 Design Studio;
- proves each capability on the physical FRDM-A-S32K358 board;
- exercises QEMU peripheral models with deterministic, reviewable tests;
- later provides reusable tests for colleague-developed ASIC models;
- retains decisions and evidence so a new engineer or agent can resume quickly.

## Scope boundaries

In scope:

- board bring-up, clock/reset/startup, FreeRTOS, interrupts, timers, communications, analog I/O, memory, multicore, and selected safety mechanisms;
- a versioned test-control protocol and a repeatable evidence process;
- comparable hardware and QEMU runs.

Initially out of scope:

- production safety certification;
- exhaustive validation of every S32K358 register;
- modifying QEMU or ASIC models before the corresponding hardware behavior is characterized;
- direct manual editing of generated RTD/S32 Configuration Tools output.

## Confirmed platform facts

Sources are the local PDFs listed in the Source register below.

- Board: FRDM-A-S32K358, board manual UM12541 Rev. 1.0 dated 8 June 2026.
- MCU: S32K358 in a 289-ball MAPBGA package, 240 MHz class, 8 MB program flash, HSE_B, ASIL-D capable device family.
- Core topology: one Cortex-M7 lockstep pair (`CM7_0` plus checker `CM7_1`) and one independent Cortex-M7 (`CM7_2`). The accepted software architecture uses AMP: one single-core FreeRTOS instance on `M7_0_0` and a separate instance on `M7_0_2`; the checker never executes tasks.
- Board debug: onboard K26 OpenSDA through USB-C; a 20-pin Cortex Debug + ETM connector is also present. External JTAG requires the documented jumper change.
- Default serial bridge: `LPUART6_RX` on PTA15 and `LPUART6_TX` on PTA16 are routed for PC serial communication.
- User outputs: RGB LED D16 on PTG29/PTG30/PTG31 and D22 on PTF21/PTF22/PTF23; LEDs are documented active-high.
- User inputs: SW2 on PTH1 and SW3 on PTH3. The manual is inconsistent about polarity: the block diagram says active-high, while section 12.2 says active-low with internal pull-ups. Hardware measurement must resolve this.
- Analog input: potentiometer R370 is routed by default to PTA17 / ADC2_S19.
- Onboard buses/devices include I2C sensors, one CAN PHY, four LIN PHYs, RMII Ethernet, 64 Mbit HyperRAM through QuadSPI, and uSDHC.
- Power-up procedure from UM12541: connect USB-C at J11, move SW5 to 2-3, and verify power indicators. A USB-C PD-capable source/cable is recommended.
- FS26 is populated in debug mode by default; watchdog/safety normal mode requires hardware population changes and is deferred.
- Reference manual: S32K3XXRM Rev. 12 dated 11 November 2025, 5,394 pages. It covers clocks, reset/boot, SIUL2, DMA, ADC, timers, LPSPI, LPI2C, FlexCAN, GMAC/EMAC, LPUART, QuadSPI, uSDHC, debug, and safety blocks.
- FreeRTOS reference supplied: kernel API reference for version 10.0.0, Issue 1. The installed PlatformSDK kernel is V11.1.0, so the supplied PDF is background vocabulary only and must not be treated as the authoritative API/configuration reference for this build.

## Workspace/tool observations (updated 2026-10-25)

- The Eclipse workspace metadata identifies S32 Design Studio 3.6.5 and NXP GCC 10.2 for Arm bare metal.
- The current generated group is `FreeRTOS_Test_SW`, containing `FreeRTOS_Test_SW_M7_0_0` and `FreeRTOS_Test_SW_M7_0_2` for the lockstep application core and independent core.
- Both current projects contain `.mex`, RTD clock/port/OS-interface sources, generated configuration, PEmicro FLASH/RAM Debug/Release launches, and C placeholder `main.c` files.
- `FreeRTOS_Test_SW_M7_0_0` is the `CORE0` image and uses the core-0 linker scripts. `FreeRTOS_Test_SW_M7_0_2` is the independent `CORE2` image and uses separate core-2 linker scripts. The two projects reference each other because they form one multicore application group.
- There is no separate `CM7_1` application project: on S32K358, `CM7_1` acts as the lockstep checker for `CM7_0` in the selected device configuration and does not execute an independently built application image.
- The two `.mex` files have the same configuration UUID and nearly identical logical content; each selects a different generated core (`M7_0_0` or `M7_0_2`). This indicates two per-core projections of one multicore device configuration, not two unrelated MCU designs.
- A controlled **Update Code and Build** cycle on 2026-10-23 normalized both `.mex` files to Clocks tool `19.0`. A direct comparison now shows only the intended core-specific differences: selected core, Pins `coreID`, and Peripherals functional-group owner.
- The projects reference PlatformSDK/RTD 7.0.0 and FreeMASTER 1.5.0. These observations must still be confirmed in the live S32DS SDK/project views before they become the frozen baseline.
- FreeMASTER is NXP's runtime instrumentation/visualization communication driver and host tool, separate from FreeRTOS. It can expose firmware variables, capture signals, tune parameters, and issue application commands over transports such as LPUART, CAN, or background debug communication.
- At the H0 snapshot, FreeMASTER source/include paths were attached to both core projects, but neither application `main.c` called `FMSTR_Init()` or `FMSTR_Poll()`; it was therefore not operational as an application service.
- On 2026-10-24 the user removed/deferred FreeMASTER from the active software baseline. It is not a current hardware-model dependency and will be added to `PERIPHERALS_USED.md` only if a later runtime integration actually uses it and its selected transport.
- Both `M7_0_0` and `M7_0_2` have cleanly regenerated `Debug_FLASH` object sets, ELF files, and map files. The H0 image totals were 54,724 bytes for core 0 and 46,444 bytes for core 2 (`text + data + bss`). The build has zero errors, but a full core-0 compile exposes 28 FreeMASTER `-Wundef` warnings plus three linker enum-ABI diagnostics involving `libc_nano.a`/`libgcc.a`. These warnings remain tracked for disposition and must not be hidden by relying only on a later incremental build summary.
- At the H0 snapshot, no FreeRTOS kernel sources or `FreeRTOSConfig.h` were present. On 2026-10-24, the NXP FreeRTOS V11.1.0 kernel, Cortex-M7 `r0p1` port, heap-1 allocator, generated `FreeRTOSConfig.h`, and FreeRTOS-backed OSIF configuration were first added to `M7_0_0`. That intermediate component/startup test did not yet start the scheduler; later runs below supersede that limitation.
- Earlier indexer summaries reported unresolved inclusions and syntax markers, but the controlled compiler builds completed with zero errors and zero warnings. Recheck the live Problems view separately; indexer markers are not compiler evidence.
- S32 Configuration Tools logged FreeRTOS component-reference, `Port_GetNumOfPinConfig`, and SIUL2 port-setting messages. Reproduce and assess these through the live Problems/build views before changing configuration.
- Hardware gate H0 passed on 2026-10-23: the primary image was reflashed and stopped in `main()` repeatably, and the generated `Debug_FLASH` group launch attached both `M7_0_0` and `M7_0_2` ELF sessions at their respective `main()` functions. Evidence and hashes are recorded in `docs/TEST_LOG.md`. The combined Phase 1/2 now introduces an independent FreeRTOS instance on each application core through staged AMP sub-gates.
- The H0 projects began as bare-metal placeholders. Both `M7_0_0` and `M7_0_2` now select the FreeRTOS OSIF and contain independent FreeRTOS V11.1.0 kernels using the Cortex-M7 `r0p1` port and heap-1. Both schedulers and all six application tasks have executed simultaneously on hardware.
- The first `M7_0_0` scheduler smoke test passed on 2026-10-24. It has since been superseded by the complete dual-core runtime proof in `RUN-20261025-015`.
- CM7_0 diagnostic hooks are now enabled and normal-path tested: the Idle hook counter advances while the application task remains notification-blocked, malloc-failure and stack-overflow Method 2 hooks link successfully, the Tick hook remains disabled, and no fatal hook was entered. Controlled negative tests for the failure hooks remain deferred.
- The first CM7_0 STM activation experiment passed on hardware on 2026-10-24. With `STM_0` channels configured for 10/100/1000 ms continuous compares, a debugger snapshot showed counts of 1037/103/10; `TaskA_10ms` executed exactly 1037 times, the Idle counter continued to advance, and the fatal state remained zero. This proves the functional STM-interrupt-to-task-notification path without yet measuring ISR-to-task latency.
- S32DS 3.6.5 includes PlatformSDK S32K3 FreeRTOS V11.1.0, the GCC Cortex-M7 `r0p1` port, and an exact `FreeRTOS_Toggle_Led_Example_S32K358`. That single-core example is the integration reference for each image. The current generated configurations express the single-core invariant with `configNUMBER_OF_CORES = 1`; this installed port does not emit a `configUSE_SMP` macro.
- An exact NXP `FreeRTOS_SMP_Example_S32K358_2xCores` is also installed, but SMP is explicitly not the selected product architecture. It may be consulted only for applicable MCU startup details.
- The implemented baseline contains six application tasks. `M7_0_0` owns `TaskA_10ms`, `TaskA_100ms`, and `TaskA_1000ms`, released by STM0 channels 0/1/2. `M7_0_2` owns `TaskB_10ms`, `TaskB_100ms`, and `TaskB_1000ms`, released by STM2 channels 0/1/2. Tasks block on ISR-safe notifications rather than self-releasing with FreeRTOS delay APIs, and preemption is enabled. CM7_0 has an enabled Idle-hook counter; CM7_2 currently has `configUSE_IDLE_HOOK=0`, so per-core Idle-hook parity remains open. Overrun reaction remains deferred while activation/execution counts are instrumented.
- `CM7_0` is the sole boot coordinator and global-initialization owner. It initializes Power/MC_RGM/PMC support and the global clock tree, then invokes the generated Power/MC_ME mode transition that releases CM7_2 at the configured `0x00700000` flash vector address. A failed global initialization skips that release. `CM7_2` performs only core-local interrupt and STM2 initialization after hardware release; it no longer polls a software `SYSTEM_READY` field.
- Boot-status protocol V3 is implemented at the fixed, 32-byte-aligned, 256-byte `NOLOAD` range `0x204BFF00..0x204BFFFF`, covered by shareable, non-cacheable MPU attributes and by CM7_0-owned startup ECC initialization through `0x204C0000`. It provides symmetric per-core `RESET/STARTING/READY/FAILED` state and fatal-error slots. Common BSP/status/fault sources are compiled into both ELFs through the linked `common_src` folder, while each image supplies its own immutable RTD binding descriptor. `RUN-20261025-017` hardware-confirms magic/version, both `READY` states, zero fatal errors, and the refactored BSP/timebase/task-diagnostics execution on both cores; warm-reset handling, primary acknowledgement reaction, and standalone no-debugger cold boot remain open.
- `RUN-20261025-017` remains the integrated dual-core runtime evidence: both schedulers and all six STM-released tasks execute simultaneously, with CPU1 counts `2506/250/25` and CPU2 counts `389/38/3`; protocol-V3 reports both cores `READY` and both fatal-error fields zero. The aligned common-source refactor and documentation were committed and pushed as `2ce2e7e` (`Refactor AMP runtime onto common support layer`). Subsequent STM characterization evidence is recorded in RUN-20261025-018 through RUN-20261025-020.
- STM characterization is implemented and IDs `1..4` have passed smoke/functional checks on both CM7_0/STM0_CH3 (`RUN-20261025-018` and `RUN-20261025-019`) and CM7_2/STM2_CH3 (`RUN-20261025-020`). Each `.mex` adds `Core0StmTestChannel` on STM0_CH3 or `Core2StmTestChannel` on STM2_CH3, preserving periodic CH0-CH2 and the existing STM prescaler. The common `stm_test` runner is advanced by the existing 10 ms task in each independent ELF and exposes debugger command IDs `1..4` for one-shot IRQ, continuous IRQ, counter monotonicity, and compare readback. STM freeze enabled in the debugger configuration resolved the observed task/timebase behavior during debugger stops; this is a debug-mode setting, not an application runtime requirement. H5 remains in progress pending remaining functional coverage, deterministic canonical results for hardware/QEMU comparison, and archived build artifacts. Physical jitter evidence is not required (D-027).

## User-confirmed prerequisites (2026-10-23)

- The physical FRDM-A-S32K358 board is working and has already been tested by the user.
- Basic board power-up and generic debug connectivity are accepted prerequisites and do not need to be repeated as discovery work.
- H0 proved that the original bare-metal images built, flashed, and reached `main()` on both application cores. Later H1 runs independently prove FreeRTOS and dual-core task execution; H0 must be interpreted only as the historical baseline gate.

## Source register

| Source | Local file | Relevant areas |
|---|---|---|
| MCU reference manual | `S32K3XXRM.pdf` | architecture, memory, clocks, reset, peripheral registers |
| Board user manual | `UM12541.pdf` | power, debug, board routing, onboard peripherals |
| FreeRTOS API reference | `FreeRTOS_Reference_Manual_V10.0.0.pdf` | kernel APIs and configuration vocabulary |
| Installed RTD GPT user manual | `C:/NXP/S32DS.3.6.5/S32DS/software/PlatformSDK_S32K3/RTD/Gpt_TS_T40D34M70I0R0/doc/RTD_GPT_UM.pdf` | STM continuous/one-shot modes, compare channels, callbacks, counters, and interrupt status |

Future documents to acquire if available: board schematic/BOM, S32K358 data sheet, S32K3 hardware design guidelines, installed RTD release notes and driver user manuals, S32DS user guide, debugger/OpenSDA guide, and the exact FreeRTOS kernel documentation matching the installed package.

## Success model

Every testable feature has two possible gates:

- `H` - hardware characterization gate: passes on the physical board with recorded evidence. Mandatory for phase completion.
- `Q` - QEMU parity gate: the same test intent passes on QEMU with an allowed-difference record. Mandatory before claiming model coverage, but may follow the hardware phase.

Possible lifecycle states: `PLANNED`, `IMPLEMENTED`, `H_PASS`, `Q_PASS`, `PARITY_PASS`, `BLOCKED`, `DEFERRED`.
