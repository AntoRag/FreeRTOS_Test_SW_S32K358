# Hardware and Model Test Log

Hardware gate H0 passed on 2026-10-23. Later phase gates remain unexecuted.

User-provided prerequisite: the FRDM-A-S32K358 board is known working and has already been tested. This is accepted as board-health context, not as a pass result for any project-specific phase gate.

## Build evidence

### BUILD-20261023-001 - Controlled per-core Update Code and Build

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:05 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | Host build only |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH`, incremental |
| Baseline Git commit | `028cc8c0657381f1ebf2f29e72cf44b0cddceca9` |
| Projects | `FreeRTOS_Test_SW_M7_0_0`, `FreeRTOS_Test_SW_M7_0_2` |
| Procedure | Select each project; execute **Update Code and Build** |
| Core 0 result | 0 errors, 0 warnings; text 44,088; data 260; bss 10,376; total 54,724 bytes |
| Core 2 result | 0 errors, 0 warnings; text 35,808; data 260; bss 10,376; total 46,444 bytes |
| Configuration result | Both `.mex` files use Clocks tool 19.0 and differ only by intended core-assignment fields |
| Outcome | PASS (incremental host build only) |
| Limitations | Not a clean rebuild; no flash, boot, debugger, or real-board evidence yet |

### BUILD-20261023-002 - Clean multicore rebuild

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:09-20:10 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | Host build only |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH`, clean rebuild followed by per-project Update Code and Build/build |
| Baseline Git commit | `f4135a7` |
| Projects | `FreeRTOS_Test_SW_M7_0_0`, `FreeRTOS_Test_SW_M7_0_2` |
| Core 0 artifacts | 54 object files; ELF 1,679,964 bytes; text 44,088; data 260; bss 10,376; total 54,724 bytes |
| Core 2 artifacts | 54 object files; ELF 1,679,720 bytes; text 35,808; data 260; bss 10,376; total 46,444 bytes |
| Errors | 0; both ELF and map files were regenerated after the clean |
| Warning review | Core 0 full-build summary: 28 compiler warnings from FreeMASTER `freemaster.h` because `FMSTR_PLATFORM_CORTEX_M` and `FMSTR_PLATFORM_S12Z` are tested while undefined under `-Wundef`. The linker also printed three enum-ABI diagnostics for `libc_nano.a`/`libgcc.a`, which were not included in Eclipse's warning count. Core 2's retained log is a later incremental 0-error/0-warning run, so its clean-build warning count was not preserved. |
| Outcome | PASS for clean artifact generation; warning disposition OPEN; hardware gate not exercised |
| Follow-up | Preserve full logs for both cores on the next clean qualification build; investigate warning remediation without editing generated or SDK-owned sources; flash and debug both images on the board. |

## Hardware evidence

### RUN-20261023-001 - H0-A primary-core debug smoke

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:16 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Onboard PEmicro/OpenSDA path selected as `USB1`; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | `c860dee` documentation state; firmware artifacts from clean build `BUILD-20261023-002` |
| Core(s) | `M7_0_0` application core with `CM7_1` lockstep checker |
| Test ID(s) | `H0-A` |
| Procedure/command | Launch `FreeRTOS_Test_SW_M7_0_0_Debug_FLASH_PNE`, then run the generated placeholder application |
| Expected result/tolerance | PEmicro connects, reset/startup script completes, application reaches `main()`, and the core runs the intentional infinite loop |
| Actual result | PEmicro debug session active; DAP successfully powered; reset script completed without a visible error; Debug view reports `Thread #1 (Running : User Request)` and the source/disassembly views identify the loop in `main()`. The TAD session log records a breakpoint hit, multiple step/resume transitions, and normal session termination. |
| Outcome | `PASS` for primary-core connect/flash/run smoke only |
| Evidence paths | `evidence/H0/2026-10-23/core0-running-main-loop.png` (SHA-256 `B4F8682A2DF765C9423F37A9405BE69E51D4FF541498DD03DD195502BBB4C060`); `evidence/H0/2026-10-23/core0-debug-session-tad.log` (SHA-256 `108E705C8A5AD92D2AAA7301F1C58AC3BB5B419CF9E4D5ECC2AE746CDE1591A7`) |
| Deviations/issues | The screenshot was captured after resume and the TAD log does not record source locations, so `exit_code == 0`, reset/reflash repeatability, and the `M7_0_2` group session remain unproven. S32DS migrated the core-0 launch schema and persisted the working `USB1` probe selection. |
| Follow-up | Suspend and inspect the primary core; restart and confirm stop at `main()`; then exercise the generated `Debug_FLASH` group launch |

### RUN-20261023-002 - H0-B multicore group launch

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:20-20:21 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Onboard PEmicro/OpenSDA path selected as `USB1`; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | `8d06720` documentation state; firmware artifacts from clean build `BUILD-20261023-002` |
| Core(s) | `M7_0_0` application core with `CM7_1` lockstep checker, plus independent `M7_0_2` |
| Test ID(s) | `H0-B` |
| Procedure/command | Launch `FreeRTOS_Test_SW_Debug_FLASH_PNE_group`; allow both generated PEmicro child launches to connect and stop |
| Expected result/tolerance | Both ELF images are associated with their intended cores and both debugger sessions stop in their respective `main()` functions without attach/reset/flash errors |
| Actual result | Group launch created `FreeRTOS_Test_SW_M7_0_0.elf` and `FreeRTOS_Test_SW_M7_0_2.elf` sessions. Both `Thread #1` entries were suspended in `main()` after stepping, at `0x0040265e` and `0x00700602` respectively. TAD logs record both DSF sessions and normal termination. The operator had also repeated the primary-core flash/reset/halt check successfully immediately before this run. |
| Outcome | `PASS` |
| Evidence paths | `evidence/H0/2026-10-23/multicore-group-halted-at-main.png` (SHA-256 `4228FABBC16B087EDF247945F73C0241C6A06290C9D4CF5BB7585A2047B5E45D`); `multicore-group-tad-main.log` (SHA-256 `A88F92EF1DA8ABB8BCA2B636DD20D0303FF5292B31F7486E12F5D68AC4929DC3`); `multicore-group-tad-secondary-end.log` (SHA-256 `C8A8224B662C96196AB2A3FF39FBBC5C0322189A2216827E94050F3F121CB0B3`) |
| Deviations/issues | Exact board PCB revision and probe firmware version remain to be recorded. Build warnings from `BUILD-20261023-002` remain open for remediation, but no compiler/linker errors or runtime-debug failure occurred. |
| Follow-up | Begin Phase 1 on the lockstep application core; keep `M7_0_2` parked except when deliberately testing multicore behavior. |

### RUN-20261024-001 - H1A FreeRTOS component and startup regression

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-24 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Existing onboard PEmicro/OpenSDA path; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0, integrated on `M7_0_0` only |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | Pre-commit working tree based on `d69612d`; resulting checkpoint commit records the exact configuration |
| Core(s) | FreeRTOS component linked into `M7_0_0`; unchanged bare-metal `M7_0_2` companion image |
| Cache/MPU state | Project startup MPU retained through `MPU_ENABLE`; FreeRTOS task MPU and MPU wrappers disabled for the scheduler baseline |
| Test ID(s) | `H1A-COMPONENT` |
| Preconditions | H0 multicore group-launch gate passed |
| Procedure/command | In S32 Configuration Tools, select the NXP FreeRTOS/CM7/heap-1 components and FreeRTOS OSIF for `M7_0_0`; disable FreeRTOS software timers and task-level MPU; generate; build `Debug_FLASH`; launch the existing multicore debug group; run and inspect both cores for startup or exception regressions |
| Expected result/tolerance | Both ELFs build; both application cores reach and run the existing placeholder application without HardFault, MemManage fault, reset, or debugger failure |
| Actual result | `FreeRTOS_Test_SW_M7_0_0.elf` linked successfully with the generated non-MPU FreeRTOS configuration. The operator reported no problem while running the existing multicore hardware debug test. |
| Outcome | `PASS` for component/link/startup regression only; scheduler and tasks are not yet exercised |
| Evidence paths | Operator observation in the project session; no new external screenshot/log archived for this sub-gate |
| Deviations/issues | Dynamic heap-1 remains the generated bring-up setting. Failure hooks, Idle hook, scheduler start, task context switching, STM notification, and FreeRTOS MPU isolation remain untested. |
| Follow-up | Commit the primary-core integration, then complete the `M7_0_0` scheduler smoke test before modifying `M7_0_2`. |

### RUN-20261024-002 - H1B FreeRTOS component and startup regression

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-24 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Existing onboard PEmicro/OpenSDA path; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0 independently integrated into `M7_0_0` and `M7_0_2` |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | Primary integration checkpoint `65e772e`; resulting secondary checkpoint commit records the exact `M7_0_2` configuration |
| Core(s) | `M7_0_0` plus independent `M7_0_2`; `CM7_1` remains the lockstep checker |
| Cache/MPU state | Startup MPU retained through `MPU_ENABLE`; FreeRTOS task MPU and wrappers disabled on both kernels for the scheduler baseline |
| Test ID(s) | `H1B-COMPONENT` |
| Preconditions | `RUN-20261024-001` passed; matching FreeRTOS settings generated for both AMP images |
| Procedure/command | Clean both projects; Build All; launch `FreeRTOS_Test_SW_Debug_FLASH_PNE_group`; confirm both cores reach their respective `main()` functions; resume both cores and observe for faults or resets |
| Expected result/tolerance | Both ELF images build and both cores run the placeholder application without compiler/linker error, HardFault, MemManage fault, assertion, unexpected reset, or debugger failure |
| Actual result | Fresh `M7_0_0` and `M7_0_2` ELF files were produced. The operator confirmed that both cores ran without observed errors in the multicore group-debug session. |
| Outcome | `PASS` for dual-kernel component/link/startup regression; neither scheduler is started yet |
| Evidence paths | Operator observation in the project session; no new external screenshot/log archived for this sub-gate |
| Deviations/issues | Dynamic heap-1 remains a documented bring-up exception. Scheduler execution, hooks, STM notification, task-level MPU isolation, and simultaneous task execution remain untested. |
| Follow-up | Commit the secondary-core integration, then start the scheduler-smoke implementation on `M7_0_0` only. |

### RUN-20261024-003 - H1A CM7_0 scheduler smoke

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-24 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Existing onboard PEmicro/OpenSDA path; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | Based on dual-kernel integration checkpoint `b5ec462`; scheduler-smoke source pending cleanup/checkpoint |
| Core(s) | Scheduler under test on `M7_0_0`; `M7_0_2` remains in its placeholder loop; `CM7_1` remains the lockstep checker |
| Cache/MPU state | Startup MPU enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-SCHED-SMOKE` |
| Preconditions | `RUN-20261024-002` passed; both independent FreeRTOS components build and pass startup regression |
| Procedure/command | Create one priority-3 CM7_0 task with a 256-word stack; start the scheduler; record task entry; block indefinitely using `ulTaskNotifyTake(pdTRUE, portMAX_DELAY)`; observe counters, handle, tick count, Idle/task state, and fault status in the debugger |
| Expected result/tolerance | Task starts exactly once and blocks without notification; release/execution counters stay zero; task handle is non-null; kernel tick advances; Idle remains available; scheduler does not return; no exception or assertion occurs |
| Actual result | Debugger observation showed `Started=1`, `ReleaseCount=0`, `ExecutionCount=0`, `FatalError=0`, a non-null task handle, and `xTickCount=1565`. The operator subsequently confirmed the blocked-task/Idle behavior appeared to work and reported no error. |
| Outcome | `PASS` for CM7_0 scheduler startup, first dispatch, notification blocking, tick progression, and Idle availability |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-scheduler-smoke-expressions.png` (SHA-256 `B3DD6FC5AEAC5C2C5D9FB30D772FD48111B20F23FC37AA89E682DD5BB78935DE`) |
| Deviations/issues | The smoke test still uses dynamic heap-1. Application-source style cleanup is required before its checkpoint. Failure hooks, stack-overflow detection, STM release, FreeRTOS MPU isolation, and CM7_2 scheduling remain untested. |
| Follow-up | Clean the CM7_0 smoke-test source, rebuild without warnings/errors, checkpoint it, then configure the first STM0 compare interrupt. |

### RUN-20261024-004 - H1A CM7_0 diagnostic hooks

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-24 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Existing onboard PEmicro/OpenSDA path; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | Based on scheduler-smoke checkpoint `a3a07ea`; resulting hook checkpoint records the exact configuration |
| Core(s) | Hooks under test on `M7_0_0`; `M7_0_2` remains in its placeholder loop |
| Cache/MPU state | Startup MPU enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-HOOKS` |
| Preconditions | `RUN-20261024-003` passed |
| Procedure/command | Enable Idle hook, malloc-failure hook, and stack-overflow Method 2 in the CM7_0 FreeRTOS configuration; keep Tick hook disabled; regenerate/build; run the multicore group; suspend CM7_0 at multiple times and inspect task, fatal, Idle, and tick counters |
| Expected result/tolerance | Idle and tick counters advance; the notification-blocked task remains started but unreleased/unexecuted; fatal state remains zero; no linker error, fault, assertion, or unexpected reset occurs |
| Actual result | Across debugger snapshots, `xTickCount` advanced from 345 to 353 and then 1058; `g_Core0IdleIterationCount` advanced from 2,298,412 to 2,354,420 and then 7,049,724. `Started` remained 1, release/execution counters remained 0, the handle remained non-null, and `FatalError` remained 0. |
| Outcome | `PASS` for Idle-hook operation and normal-path integration of the configured diagnostic hooks |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-hooks-tick-0345.png` (SHA-256 `F5FBBC55D490C192C212661303F55BAEA6CAC4BFE37160485DEF59F5A9248434`); `cm7-0-hooks-tick-0353.png` (SHA-256 `8FE726D1D74E05B6D5FD69D43D876DB03CED10E6F14FAE197A85268D9A0B89B1`); `cm7-0-hooks-tick-1058.png` (SHA-256 `630B7F006FE2722ED26E8FD2B53AF52E505968283C1323DB6F58850E621CD7B0`) |
| Deviations/issues | Malloc-failure and stack-overflow hooks were linked but not deliberately triggered. Dynamic heap-1 remains a bring-up exception. |
| Follow-up | Checkpoint the hook configuration, then configure the first CM7_0 STM compare channel for a 10 ms task release. |

Copy the template below for each executed test. Do not overwrite earlier evidence; append a new run.

## Run template

### RUN-YYYYMMDD-NNN - `<test ID or phase gate>`

| Field | Value |
|---|---|
| Date/time/timezone | |
| Operator | |
| Target | `HARDWARE` / `QEMU` / `ASIC_MODEL` |
| Board/model revision | |
| Board setup/jumpers | |
| Power source/cable | |
| Probe and firmware | |
| S32DS version | |
| RTD/SDK version | |
| FreeRTOS version | |
| Compiler/version | |
| Build configuration | |
| Git commit/build ID | |
| Core(s) | |
| Clock configuration | |
| Cache/MPU state | |
| Test ID(s) | |
| Preconditions | |
| Procedure/command | |
| Expected result/tolerance | |
| Actual result | |
| Outcome | `PASS` / `FAIL` / `SKIP` / `ERROR` / `TIMEOUT` |
| Evidence paths | |
| Deviations/issues | |
| Follow-up | |

### Evidence checklist

- build console log;
- ELF and map-file identity/size;
- flash/debug console result;
- raw UART/test-protocol transcript;
- scope, logic analyzer, CAN/LIN analyzer, or packet capture where relevant;
- board photo/setup diagram when wiring changes;
- QEMU command line and model revision for model runs;
- allowed-difference explanation for parity judgments.

## Phase-gate summary

| Gate | Status | Latest run | Notes |
|---|---|---|---|
| H0 Reproducible baseline | PASS | RUN-20261023-002 | Clean build artifacts, repeatable primary flash/reset/halt, and multicore group launch verified on hardware |
| H1/2 Dual-core AMP FreeRTOS and observability | IN PROGRESS | RUN-20261024-004 | CM7_0 scheduler and Idle/diagnostic-hook normal path passed; negative-hook tests, FreeRTOS MPU gate, STM release, CM7_2 scheduling, and simultaneous AMP task execution remain open |
| H3 Test harness | PLANNED | - | |
| H4 GPIO/interrupts | PLANNED | - | |
| H5 Timers/timebase | PLANNED | - | |
| H6 LPUART stress | PLANNED | - | |
| H7 ADC/I2C | PLANNED | - | |
| H8 LPSPI/eDMA | PLANNED | - | |
| H9 CAN/LIN | PLANNED | - | |
| H10 Ethernet/storage/memory | PLANNED | - | |
| H11 Multicore | PLANNED | - | |
| H12 Reset/power/safety | PLANNED | - | |
| Parity regression | PLANNED | - | |
