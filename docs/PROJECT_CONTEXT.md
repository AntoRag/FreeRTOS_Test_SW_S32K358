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
- Core topology: one Cortex-M7 lockstep pair (`CM7_0` plus checker `CM7_1`) and one independent Cortex-M7 (`CM7_2`). Initial software should stay on the lockstep application core until multicore is introduced deliberately.
- Board debug: onboard K26 OpenSDA through USB-C; a 20-pin Cortex Debug + ETM connector is also present. External JTAG requires the documented jumper change.
- Default serial bridge: `LPUART6_RX` on PTA15 and `LPUART6_TX` on PTA16 are routed for PC serial communication.
- User outputs: RGB LED D16 on PTG29/PTG30/PTG31 and D22 on PTF21/PTF22/PTF23; LEDs are documented active-high.
- User inputs: SW2 on PTH1 and SW3 on PTH3. The manual is inconsistent about polarity: the block diagram says active-high, while section 12.2 says active-low with internal pull-ups. Hardware measurement must resolve this.
- Analog input: potentiometer R370 is routed by default to PTA17 / ADC2_S19.
- Onboard buses/devices include I2C sensors, one CAN PHY, four LIN PHYs, RMII Ethernet, 64 Mbit HyperRAM through QuadSPI, and uSDHC.
- Power-up procedure from UM12541: connect USB-C at J11, move SW5 to 2-3, and verify power indicators. A USB-C PD-capable source/cable is recommended.
- FS26 is populated in debug mode by default; watchdog/safety normal mode requires hardware population changes and is deferred.
- Reference manual: S32K3XXRM Rev. 12 dated 11 November 2025, 5,394 pages. It covers clocks, reset/boot, SIUL2, DMA, ADC, timers, LPSPI, LPI2C, FlexCAN, GMAC/EMAC, LPUART, QuadSPI, uSDHC, debug, and safety blocks.
- FreeRTOS reference supplied: kernel API reference for version 10.0.0, Issue 1. The actual kernel version installed/selected in S32DS remains to be confirmed and must not be assumed from this manual.

## Workspace/tool observations (2026-10-23)

- The Eclipse workspace metadata identifies S32 Design Studio 3.6.5 and NXP GCC 10.2 for Arm bare metal.
- The current generated group is `FreeRTOS_Test_SW`, containing `FreeRTOS_Test_SW_M7_0_0` and `FreeRTOS_Test_SW_M7_0_2` for the lockstep application core and independent core.
- Both current projects contain `.mex`, RTD clock/port/OS-interface sources, generated configuration, PEmicro FLASH/RAM Debug/Release launches, and C placeholder `main.c` files.
- `FreeRTOS_Test_SW_M7_0_0` is the `CORE0` image and uses the core-0 linker scripts. `FreeRTOS_Test_SW_M7_0_2` is the independent `CORE2` image and uses separate core-2 linker scripts. The two projects reference each other because they form one multicore application group.
- There is no separate `CM7_1` application project: on S32K358, `CM7_1` acts as the lockstep checker for `CM7_0` in the selected device configuration and does not execute an independently built application image.
- The two `.mex` files have the same configuration UUID and nearly identical logical content; each selects a different generated core (`M7_0_0` or `M7_0_2`). This indicates two per-core projections of one multicore device configuration, not two unrelated MCU designs.
- Without manual edits by the user, the generated `.mex` files currently report different Clocks-tool revisions (`15.0` for core 0 and `19.0` for core 2) and some different calculated clock outputs. Generated clock/port files also differ. Treat this as project-template/tool-generation provenance to normalize and validate before freezing the baseline, not as a user configuration error.
- The projects reference PlatformSDK/RTD 7.0.0 and FreeMASTER 1.5.0. These observations must still be confirmed in the live S32DS SDK/project views before they become the frozen baseline.
- FreeMASTER is NXP's runtime instrumentation/visualization communication driver and host tool, separate from FreeRTOS. It can expose firmware variables, capture signals, tune parameters, and issue application commands over transports such as LPUART, CAN, or background debug communication.
- FreeMASTER source/include paths are attached to both current core projects, but neither application `main.c` currently calls `FMSTR_Init()` or `FMSTR_Poll()`. Therefore it is present in the build configuration but not yet operational as an application service.
- Both `M7_0_0` and `M7_0_2` now contain `Debug_FLASH` ELF and map artifacts. Their presence shows that build outputs have been produced, but a clean `Update Code and Build` result and real-board execution are still required before accepting the baseline.
- No FreeRTOS kernel sources or `FreeRTOSConfig.h` were found in the generated project tree at the snapshot time, despite FreeRTOS-related S32 Configuration Tools log messages. Confirm component selection and generated output before calling this a FreeRTOS project.
- Latest indexer summaries reported five unresolved inclusions and two syntax errors per project. These are indexer/configuration observations, not compiler build results.
- S32 Configuration Tools logged FreeRTOS component-reference, `Port_GetNumOfPinConfig`, and SIUL2 port-setting messages. Reproduce and assess these through the live Problems/build views before changing configuration.

## User-confirmed prerequisites (2026-10-23)

- The physical FRDM-A-S32K358 board is working and has already been tested by the user.
- Basic board power-up and generic debug connectivity are accepted prerequisites and do not need to be repeated as discovery work.
- This confirmation does not yet prove that the current `FreeRTOS_Test_SW` configuration builds, flashes, boots, or runs FreeRTOS correctly; those remain project-specific gates.

## Source register

| Source | Local file | Relevant areas |
|---|---|---|
| MCU reference manual | `S32K3XXRM.pdf` | architecture, memory, clocks, reset, peripheral registers |
| Board user manual | `UM12541.pdf` | power, debug, board routing, onboard peripherals |
| FreeRTOS API reference | `FreeRTOS_Reference_Manual_V10.0.0.pdf` | kernel APIs and configuration vocabulary |

Future documents to acquire if available: board schematic/BOM, S32K358 data sheet, S32K3 hardware design guidelines, installed RTD release notes and driver user manuals, S32DS user guide, debugger/OpenSDA guide, and the exact FreeRTOS kernel documentation matching the installed package.

## Success model

Every testable feature has two possible gates:

- `H` - hardware characterization gate: passes on the physical board with recorded evidence. Mandatory for phase completion.
- `Q` - QEMU parity gate: the same test intent passes on QEMU with an allowed-difference record. Mandatory before claiming model coverage, but may follow the hardware phase.

Possible lifecycle states: `PLANNED`, `IMPLEMENTED`, `H_PASS`, `Q_PASS`, `PARITY_PASS`, `BLOCKED`, `DEFERRED`.
