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

## Current software and tool baseline (2026-10-25)

This section describes the current checkout. Dated observations and results remain in `TEST_LOG.md`; release manifests identify packaged sources and binaries.

- The recorded tool baseline is S32 Design Studio 3.6.5, S32K3 PlatformSDK/RTD 7.0.0, NXP GCC Arm bare metal 10.2, and FreeRTOS V11.1.0 with the Cortex-M7 `r0p1` port. Both `.mex` files use schema version 19 and Clocks document version 19.0. Exact board PCB revision and probe firmware remain unrecorded.
- The workspace contains two buildable core projects, `FreeRTOS_Test_SW_M7_0_0` and `FreeRTOS_Test_SW_M7_0_2`, plus the non-building `FreeRTOS_Test_SW_Common` project for browsing common files. Each core project independently compiles `../common/src` through its `common_src` linked source folder and includes `../common/include`.
- The two `.mex` files share a configuration UUID, but contain intentional per-image differences: core selection, interrupt/STM bindings, primary-only Power configuration, and diagnostic-hook settings. They are not currently identical except for a core-ID field. Each physical peripheral/global initialization action has one owner (D-010, D-017).
- Only the FLASH PEmicro Launch Group and its two child launches are tracked. The group is `FreeRTOS_Test_SW_Debug_FLASH_PNE_group`; the two children target their corresponding `Debug_FLASH` ELF images. Other managed-build configurations do not imply a validated RAM or Release debug workflow.
- Both images contain independent single-core FreeRTOS kernels (`configNUMBER_OF_CORES=1`); SMP is rejected. The generated configurations set a 1 kHz tick and `configCPU_CLOCK_HZ=160000000`. Task-level FreeRTOS MPU support is disabled; the project startup MPU and cache configuration remain enabled.
- Six application tasks execute: TaskA at 10/100/1000 ms on CM7_0, and TaskB at the same rates on CM7_2. STM0 and STM2 respectively provide 1 MHz counters and continuous CH0/CH1/CH2 releases through ISR-safe task notifications. Preemption is enabled. Task creation currently uses `xTaskCreate()` and heap-1 with dynamic allocation; static allocation is a proposed later migration, not the present implementation.
- CM7_0 enables the Idle hook, malloc-failure hook, and stack-overflow Method 2. CM7_2 currently disables all three generated hook options despite containing hook function definitions. Thus a zero CM7_2 Idle counter is expected; hook parity and controlled negative tests remain open.
- CM7_0 alone performs system-global Power/MC_RGM/PMC and clock initialization, then releases CM7_2 through Power/MC_ME at the configured flash vector address `0x00700000`. A failed global initialization skips release. CM7_2 publishes its own startup status and initializes local resources without polling `SYSTEM_READY` (D-024, D-025).
- Protocol V3 exposes `g_AppSystemStatus` in the 256-byte `.boot_mailbox` `NOLOAD` range `0x204BFF00..0x204BFFFF`, with two `RESET/STARTING/READY/FAILED` state/error slots. CM7_0 owns initial shared-SRAM ECC initialization through exclusive end `0x204C0000`. Both slots were observed `READY` with fatal error zero in `RUN-20261025-017`; failure publication, warm-reset handling, and primary supervision remain unqualified.
- `RUN-20261025-016/017` prove the common-source runtime refactor and exact observed interrupt-to-task count equality on both cores. Commit `2ce2e7e` checkpoints that implementation. These runs used the multicore debugger; standalone no-debugger cold boot and defined soak/repetition evidence remain open.
- STM test software on CH3 of each core-owned instance is implemented and functionally hardware-passed for IDs `STM-001..004` (`RUN-20261025-018/019/020`). The existing 10 ms tasks advance the common `stm_test` runner. Each ELF has its own command/result globals. Current tests cover one-shot IRQ, continuous IRQ, nonzero modular counter deltas, and compare readback; remaining functional coverage and exact-value QEMU parity are open.
- Both `.mex` files set `StmFreezeEnable=true`; generated STM configuration causes firmware initialization to program the hardware debug-freeze bit. This setting resolved the reported halt/resume task behavior. It affects debugger halt behavior and is not a debugger launch checkbox (D-021).
- The recorded PEmicro plug-in is `com.pemicro.debug.gdbjtag.pne_6.2.7.202610202056`. Its conflicting stock `S32K358.mac` is disabled by a host-local rename to `S32K358_.mac` (D-023). Firmware owns SRAM ECC initialization rather than depending on the disabled macro.
- FreeMASTER is deferred: no FreeMASTER service is initialized or polled. Retained SDK/header files are not a runtime dependency. The proposed UART command/result transport and LED reporting are not implemented.
- Versioned packages `v0.1.0` and `v0.2.0` are committed under `release/`; both contain source, two ELF/map pairs, launch files, and hash manifests. `v0.2.0` introduces the STM test software. See `RELEASES.md` for exact source/artifact commits and provenance limits. A release package does not retroactively identify binaries used in older screenshot-only runs.

Use `BUILD_AND_DEBUG.md` for the development workflow and `test_sw/STM.md` for component-specific operation. The repository README is an overview and navigation page (D-028).

## Historical baseline notes

H0 passed on 2026-10-23 with the original bare-metal placeholder images. The H0 build sizes and FreeMASTER/enum-ABI warning observations apply to that historical baseline (`BUILD-20261023-001/002`), not the current firmware. Later functional runs prove scheduler/task behavior, but do not replace missing full build-console logs. Compiler diagnostics, Eclipse indexer markers, flashing results, and board tests remain separate facts.

The installed NXP `FreeRTOS_Toggle_Led_Example_S32K358` supplies the accepted per-image integration reference. The installed SMP example may be consulted only for relevant hardware-startup details; its scheduler architecture is not selected.

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
- `Q` - QEMU execution gate: the same test software passes on QEMU; compare canonical outputs with the hardware reference to claim `PARITY_PASS` (D-027). Unsupported behavior is recorded as a gap and does not count as parity. These gates may follow the hardware phase.

Possible lifecycle states: `PLANNED`, `IMPLEMENTED`, `H_PASS`, `Q_PASS`, `PARITY_PASS`, `BLOCKED`, `DEFERRED`.
