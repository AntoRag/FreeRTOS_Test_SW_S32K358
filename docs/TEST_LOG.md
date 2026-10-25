# Hardware and Model Test Log

Hardware gate H0 passed on 2026-10-23. The dual-core AMP scheduler/task runtime sub-gate passed on 2026-10-25 in `RUN-20261025-015` and is checkpointed by commit `49b4ad7`. The complete combined H1/2 gate remains in progress because not all roadmap exit criteria have been exercised.

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

### RUN-20261024-005 - H1A CM7_0 STM periodic task release

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
| Git commit/build ID | Uncommitted STM working tree based on the earlier H1A checkpoints; exact reproducible identity remains pending commit |
| Core(s) | STM and task-release experiment on `M7_0_0` |
| Clock configuration | FreeRTOS tick 1 kHz; `STM_0` counter configured at 1 MHz; continuous compare periods 10,000, 100,000, and 1,000,000 counts |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-STM-TASKA10MS` |
| Preconditions | `RUN-20261024-004` passed; STM0 channels 0/1/2 and `STM0_IRQn` configured; `TaskA_10ms` blocks on a direct task notification |
| Procedure/command | Build and flash the current group, let the target run, suspend it in S32DS, and inspect the FreeRTOS tick, fatal state, Idle counter, STM channel interrupt counters, task handle, and `TaskA_10ms` execution count |
| Expected result/tolerance | Fatal state remains zero; Idle activity continues; all three STM counters advance at nominal 10/100/1000 ms ratios; every channel-0 interrupt releases one `TaskA_10ms` execution with no accumulated backlog under the no-load test |
| Actual result | At the captured halt, `xTickCount=10331`, `g_Core0Stm10msInterruptCount=1037`, `g_Core0Stm100msInterruptCount=103`, `g_Core0Stm1000msInterruptCount=10`, `g_Core0TaskA10msExecutionCount=1037`, `g_Core0IdleIterationCount=68751812`, the task handle was non-null, and `g_Core0FatalError=0`. The observed interrupt ratios match the configured periods, and the 10 ms interrupt and task-execution counts match exactly. |
| Outcome | `PASS` for the functional `STM_0 CH0 -> ISR notification -> TaskA_10ms` path and simultaneous 10/100/1000 ms STM cadence |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-stm-taska10ms-pass.png` (SHA-256 `2C73DE9EC7570E6F76063B8A2918EC7DCD311F0B40C6F681F9FE6C739CAE7692`) |
| Deviations/issues | A debugger counter snapshot proves functional delivery and absence of backlog in this run, but does not measure ISR-to-task latency or independently prove immediate preemption before the next kernel tick. Exact board/probe versions and the build log remain to be archived. |
| Follow-up | Perform a behavior-preserving source split and repeat this regression; then add and independently verify `TaskA_100ms`, followed by `TaskA_1000ms`. |

### RUN-20261024-006 - H1A source-split regression

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
| Git commit/build ID | Uncommitted refactored working tree; checkpoint pending |
| Core(s) | Refactored application on `M7_0_0` |
| Clock configuration | FreeRTOS tick 1 kHz; `STM_0` counter 1 MHz; continuous compare periods 10,000, 100,000, and 1,000,000 counts |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-SOURCE-SPLIT-REGRESSION` |
| Preconditions | `RUN-20261024-005` passed; application code split into fault, task, board-initialization, timebase, and FreeRTOS-hook modules |
| Procedure/command | Build and flash the refactored CM7_0 application, run it on the board, suspend it in S32DS, and inspect the same task, STM, tick, Idle, and fatal-state variables used in the pre-refactor test |
| Expected result/tolerance | The refactor preserves behavior: fatal state remains zero; Idle and tick counts advance; the STM counters retain nominal 10:1 and 100:1 relationships; and the 10 ms interrupt count equals the `TaskA_10ms` execution count |
| Actual result | At the captured halt, `xTickCount=19463`, `g_Core0Stm10msInterruptCount=1954`, `g_Core0Stm100msInterruptCount=195`, `g_Core0Stm1000msInterruptCount=19`, `g_Core0TaskA10msExecutionCount=1954`, `g_Core0IdleIterationCount=129516624`, the task handle was non-null, and `g_Core0FatalError=0`. The interrupt ratios remain nominal and the 10 ms interrupt-to-task counts match exactly. |
| Outcome | `PASS` for behavior preservation across the source split |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-source-split-regression-pass.png` (SHA-256 `2428A79A3F5530EA7309D5C0AB01BC214F1F9F568B869A269727FD11E2E5ED21`) |
| Deviations/issues | This is debugger-snapshot evidence; an archived clean-build console log and precise interrupt-to-task latency measurement remain open. The next experiment intentionally introduces both remaining CM7_0 tasks together at the operator's request, rather than the normal one-variable-per-board-experiment preference. |
| Follow-up | Complete the minor application-interface cleanup, then add `TaskA_100ms` and `TaskA_1000ms` and verify that each task-execution counter matches its dedicated STM-channel interrupt counter. |

### RUN-20261024-007 - H1A complete CM7_0 periodic task set

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
| Git commit/build ID | Uncommitted CM7_0 three-task working tree; checkpoint pending |
| Core(s) | Complete `TaskA` periodic set on `M7_0_0` |
| Clock configuration | FreeRTOS tick 1 kHz; `STM_0` counter 1 MHz; continuous compare periods 10,000, 100,000, and 1,000,000 counts |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-TASKA-PERIODIC-SET` |
| Preconditions | `RUN-20261024-006` passed; all three task handles are created before the STM channels start; task priorities are rate-monotonic (`10 ms > 100 ms > 1000 ms > Idle`) |
| Procedure/command | Add `TaskA_100ms` and `TaskA_1000ms` to the existing `TaskA_10ms` application, connect each STM callback to its dedicated task notification, build and flash, run on the board, suspend CM7_0, and inspect the task-execution, interrupt, tick, Idle, and fatal-state counters |
| Expected result/tolerance | Fatal state remains zero; Idle and kernel tick advance; STM counts retain nominal 100:10:1 cadence; every task-execution count equals its corresponding interrupt count, allowing a transient difference of one if halted between ISR entry and task execution |
| Actual result | At the captured halt, `xTickCount=34443`, `g_Core0Stm10msInterruptCount=3460`, `g_Core0Stm100msInterruptCount=346`, `g_Core0Stm1000msInterruptCount=34`, and the corresponding task-execution counts were exactly `3460`, `346`, and `34`. `g_Core0IdleIterationCount=229182094`, the displayed 10 ms task handle was non-null, and `g_Core0FatalError=0`. Execution of the 100 ms and 1000 ms tasks also demonstrates that their handles were valid when notified. |
| Outcome | `PASS` for the complete CM7_0 three-rate periodic task skeleton |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-three-periodic-tasks-pass.png` (SHA-256 `DCEFC1F7A214812FDF8130248742D32A363436BBAFF0A3CB3D331748A506EFD3`) |
| Deviations/issues | The 100 ms and 1000 ms tasks were introduced together by explicit operator choice after their STM channels had already been independently exercised. Counter equality proves functional releases without observed backlog in this run, but does not measure interrupt-to-task latency. Minor public-interface naming/comment cleanup remains before checkpointing. A clean-build log was not archived. |
| Follow-up | Apply the remaining naming/interface cleanup, perform one final build/regression, checkpoint the CM7_0 task set, then implement the independent `TaskB` set on `M7_0_2`. |

### RUN-20261024-008 - H1A interface-cleanup regression

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
| Git commit/build ID | Uncommitted CM7_0 cleanup working tree; checkpoint pending |
| Core(s) | Complete `TaskA` periodic set on `M7_0_0` |
| Clock configuration | FreeRTOS tick 1 kHz; `STM_0` counter 1 MHz; continuous compare periods 10,000, 100,000, and 1,000,000 counts |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-INTERFACE-CLEANUP-REGRESSION` |
| Preconditions | `RUN-20261024-007` passed; public counter declarations, task creation prototype, and per-channel task comments were cleaned up |
| Procedure/command | Rebuild and run the modified application on the board, suspend CM7_0, and compare all three STM interrupt counters with their corresponding task-execution counters while checking tick, Idle, and fatal state |
| Expected result/tolerance | The source-only cleanup preserves the complete periodic-task behavior and produces no fatal state |
| Actual result | At the captured halt, `xTickCount=37678`, STM interrupt counts were `3623`, `362`, and `36`, and the corresponding task-execution counts were exactly `3623`, `362`, and `36`. `g_Core0IdleIterationCount=250719941`, the displayed 10 ms task handle was non-null, and `g_Core0FatalError=0`. |
| Outcome | `PASS` for functional behavior preservation after the interface cleanup |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-cleanup-regression-pass.png` (SHA-256 `8DF3C7602E912A47BD9C951E580A77BB47B2BF4EEBF26CC17B93B92A66E0BED1`) |
| Deviations/issues | After the captured hardware run, the three release APIs received the final symbol-only capitalization correction from `...FromIsr` to the accepted `...FromISR` convention. Static inspection confirmed that declarations, definitions, and callers agree and that no stale `FromIsr` symbol remains. The operator subsequently confirmed a successful rebuild. A clean-build console log was not archived. |
| Follow-up | Checkpoint the CM7_0 task set, then begin the independent CM7_2 `TaskB` implementation. |

### RUN-20261024-009 - H1A global/core-local initialization split

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
| Git commit/build ID | Initialization-split working tree based on `175cd4b`; checkpoint pending |
| Core(s) | `M7_0_0`; `M7_0_2` application remains unchanged |
| Clock configuration | FreeRTOS tick 1 kHz; `STM_0` counter 1 MHz; continuous compare periods 10,000, 100,000, and 1,000,000 counts |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-INIT-OWNERSHIP-SPLIT` |
| Preconditions | `RUN-20261024-008` passed; D-017 assigns system-global initialization to `CM7_0` and preserves per-core local initialization |
| Procedure/command | Replace the combined `Bsp_Init()` with primary-only `Bsp_SystemInit()` containing `Clock_Ip_Init()` and local `Bsp_CoreInit()` containing `IntCtrl_Ip_Init()`; rebuild, flash, run on hardware, and capture two debugger snapshots of the existing CM7_0 observability variables |
| Expected result/tolerance | The structural split preserves all previously qualified CM7_0 behavior: no fatal error, advancing tick and Idle activity, nominal 10:1 and 100:1 STM cadence, and equality between every STM interrupt count and its corresponding task-execution count |
| Actual result | Snapshot 1: `xTickCount=2823`, Idle `18788144`, interrupt counts `283/28/2`, task counts `283/28/2`, fatal `0`. Snapshot 2: `xTickCount=5797`, Idle `38577699`, interrupt counts `581/58/5`, task counts `581/58/5`, fatal `0`. Both samples show exact interrupt-to-task equality and continued progression after the initialization split. Static inspection confirmed that no call to the old `Bsp_Init()` remains. |
| Outcome | `PASS` for the CM7_0 global/core-local initialization split |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-init-split-snapshot-1.png` (SHA-256 `3481D2387521425BEFF7469AE5B61ADF7008258FD36CB4BCC33BD85BC3A3EFA0`); `evidence/H1/2026-10-24/cm7-0-init-split-snapshot-2.png` (SHA-256 `7B75FC2B791F28FEAC38FD4ACA15944907510B1E9DFFCB7B7D757A251B49B412`) |
| Deviations/issues | This run validates the primary-image refactor only. `SYSTEM_READY`, standalone CM7_2 release, the secondary local-initialization path, and the secondary STM/task set are not implemented yet. A clean-build console log was not archived. |
| Follow-up | Checkpoint the split, define and test the bounded `SYSTEM_READY` startup barrier, then configure the CM7_2-owned STM and implement `TaskB`. |

### RUN-20261024-010 - H1A logical boot/application entrypoint split

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
| Git commit/build ID | Uncommitted entrypoint-refactor working tree; checkpoint pending |
| Core(s) | Refactored logical boot/application entrypoints on `M7_0_0`; `M7_0_2` remains unchanged |
| Clock configuration | FreeRTOS tick 1 kHz; `STM_0` counter 1 MHz; continuous compare periods 10,000, 100,000, and 1,000,000 counts |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-ENTRYPOINT-SPLIT-REGRESSION` |
| Preconditions | `RUN-20261024-009` passed; D-019 accepted; generated `Reset_Handler -> main()` runtime path retained |
| Procedure/command | Refactor CM7_0 into the logical path `main() -> Core0_Boot() -> Core0_Main()` without changing generated startup or linker entry; rebuild and run on hardware; suspend twice and inspect fatal, tick, Idle, STM-interrupt, and TaskA-execution counters |
| Expected result/tolerance | The refactor preserves behavior: `g_Core0FatalError=0`; tick and Idle advance; every STM interrupt count equals its matching TaskA execution count, allowing a transient difference of one when halted between ISR and task execution |
| Actual result | Snapshot 1: `xTickCount=5455`, Idle `36301542`, interrupt counts `547/54/5`, task counts `547/54/5`, fatal `0`, and the displayed 10 ms task handle was non-null. Snapshot 2: `xTickCount=8961`, Idle `59629046`, interrupt counts `899/89/8`, task counts `899/89/8`, fatal `0`, and the task handle remained non-null. The first attempted run correctly exposed an inverted `main()` success condition; after changing it to `faultCode == APP_FATAL_NONE` and removing the redundant scheduler-error assignment, the regression passed. |
| Outcome | `PASS` for behavior preservation through the CM7_0 logical boot/application entrypoint split |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-entrypoint-split-snapshot-1.png` (SHA-256 `989320D8DE2441B9A86AF9D87B7D16EB105E73A2EA54CADDBB9AE9DACD6187FE`); `cm7-0-entrypoint-split-snapshot-2.png` (SHA-256 `ED220B2A30429F83E5C9C1D48613A56F3265DAA64F748DF1F02038AE7C932472`) |
| Deviations/issues | A clean-build console log was not archived, so zero-warning build evidence remains unconfirmed. The mailbox, `MC_ME` release, CM7_2 boot wait, and `Core2_Main()` are not part of this test. |
| Follow-up | Implement the shared boot-mailbox module and integrate only its CM7_0 initialization into `Core0_Boot()` before adding CM7_2 consumption or MC_ME release. |

### RUN-20261024-011 - H1A CM7_0 boot-mailbox publication

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
| Git commit/build ID | Uncommitted boot-mailbox working tree; checkpoint pending |
| Core(s) | Mailbox producer on `M7_0_0`; `M7_0_2` consumer not yet implemented |
| Shared-memory configuration | Protocol V1 mailbox at `0x204BFF00`, 256 bytes, `NOLOAD`, 32-byte aligned, covered by the generated shareable/non-cacheable MPU region |
| Test ID(s) | `H1A-BOOT-MAILBOX-PUBLISH` |
| Preconditions | `RUN-20261024-010` passed; D-020 accepted; map shows `.boot_mailbox` and `g_BootSyncMailbox` at `0x204BFF00` with size `0x100` and both linker assertions satisfied |
| Procedure/command | Initialize every mailbox word from `Core0_Boot()`, execute the global clock initialization, publish `SYSTEM_READY` using the producer memory barrier, run on hardware, suspend CM7_0, and inspect the complete mailbox header together with the existing TaskA/STM observability variables |
| Expected result/tolerance | Magic `0x4253594E`, protocol version `1`, system state `READY (2)`, system error `0`, CM7_2 state `RESET (0)`, and CM7_2 error `0`; fatal state remains zero and every STM interrupt count equals its corresponding TaskA execution count |
| Actual result | Debugger observation showed magic `0x4253594E`, protocol version `1`, system state `2`, system error `0`, CM7_2 state `0`, and CM7_2 error `0`. `g_Core0FatalError=0`, `xTickCount=2612`, Idle `17386577`, STM counts `261/26/2`, and matching TaskA counts `261/26/2`; the displayed 10 ms task handle was non-null. The earlier enum-domain warning was corrected by publishing `BOOT_SYNC_SYSTEM_ERROR_CLOCK_INIT` to the mailbox while retaining `APP_FATAL_CLOCK_INIT` for the local application fault. |
| Outcome | `PASS` for CM7_0 mailbox initialization, valid protocol publication, fixed runtime placement, and preservation of the complete TaskA timing behavior |
| Evidence paths | `evidence/H1/2026-10-24/cm7-0-mailbox-publish-pass.png` (SHA-256 `354CB2DDC436708F795725DE3059BAB90E3D76A5A0C9B5F52BEC42DA96C90C7B`) |
| Deviations/issues | Cross-core visibility is not proven because CM7_2 does not yet consume the mailbox. The clock-failure publication path, stale-state handling, polling timeout, and warm-reset behavior have not been exercised. A clean-build console log was not archived. |
| Follow-up | Implement `Core2_BootWait()` as a bounded consumer of the mailbox while retaining the current debugger/group-launch physical release; verify valid READY consumption before adding CM7_2 local initialization or MC_ME release. |

### RUN-20261024-012 - H1 CM7_2 boot-mailbox consumption

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
| Git commit/build ID | Uncommitted dual-image boot-mailbox working tree; checkpoint pending |
| Core(s) | `M7_0_0` mailbox producer and `M7_0_2` bounded consumer |
| Shared-memory configuration | Protocol V1 mailbox at `0x204BFF00`, 256 bytes, `NOLOAD`, 32-byte aligned, shareable and non-cacheable; CM7_2 map reports `.boot_mailbox` size `0x100` from `boot_sync_secondary.o` and both linker assertions pass |
| Test ID(s) | `H1-CROSS-CORE-BOOT-MAILBOX-CONSUME` |
| Preconditions | `RUN-20261024-011` passed; both images built; CM7_0 was allowed to initialize and publish the mailbox before CM7_2 executed its consumer path |
| Procedure/command | Launch both hardware debug sessions, retain the secondary until CM7_0 has published `SYSTEM_READY`, resume CM7_2, suspend it after entry to `Core2_Main()`, and inspect the CM7_2 boot diagnostics and its view of the shared mailbox |
| Expected result/tolerance | CM7_2 reports `BOOT_SYNC_WAIT_READY`, performs at least one poll, enters `Core2_Main()`, and observes magic `0x4253594E`, protocol version `1`, `systemState=READY (2)`, and `systemError=0` |
| Actual result | CM7_2 observed magic `0x4253594E`, protocol version `1`, `systemState=2`, and `systemError=0`; `g_Core2BootWaitStatus=BOOT_SYNC_WAIT_READY`, `g_Core2BootPollCount=1`, and `g_Core2MainEntered=1`. The mailbox retained `core2State=RESET (0)` and `core2Error=0` as expected because CM7_2 publication is not implemented yet. A CM7_0-only TaskA expression shown in the CM7_2 debug context could not be evaluated; this is expected per-ELF symbol scoping and is not a runtime failure. |
| Outcome | `PASS` for successful-path cross-core mailbox visibility, protocol validation, bounded consumer completion, and CM7_2 application entry |
| Evidence paths | `evidence/H1/2026-10-24/cm7-2-mailbox-consume-pass.png` (SHA-256 `28B436F930F016AD11F4927AE488E1C85C87FCCA9572D01DBBEDF78803A95660`) |
| Deviations/issues | Active polling is an intentional bootstrap implementation and is not the preferred final wake-up mechanism. The polling count is an iteration bound rather than calibrated time. Timeout, invalid protocol/state, primary-failure, warm-reset/stale-mailbox, `CORE2_READY`, `MC_ME` physical release, and hardware-assisted notification paths remain untested. A clean-build log was not archived. |
| Follow-up | Add CM7_2-owned `WAITING`/`READY`/`FAILED` publication and local initialization before TaskB; retain the mailbox as the state contract and evaluate `MSCM` directed interrupt or `MU` event wake-up in Phase 11. |

### RUN-20261024-013 - STM debug-freeze isolation

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
| Build configuration | `Debug_FLASH`; temporary diagnostic launch with only the primary debug session active |
| Git commit/build ID | Uncommitted diagnostic configuration; checkpoint pending |
| Core(s) | `M7_0_0` executing while the debugger left `M7_0_2` halted |
| Clock configuration | FreeRTOS tick 1 kHz; `STM_0` nominally 1 MHz; clock/MC_ME register observations unchanged between the failing and working diagnostic cases |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; FreeRTOS task MPU disabled |
| Test ID(s) | `H1A-STM-DEBUG-FREEZE-DIAGNOSIS` |
| Preconditions | The Launch Group executed TaskA correctly, while a primary-only debug launch advanced the Idle counter but left `STM_0.CNT`, STM interrupt counters, and TaskA execution counters stopped. `StmFreezeEnable` was initially enabled and generated `STM_0.CR = 0x2F03`. |
| Procedure/command | Change only `StmFreezeEnable` to false through S32 Configuration Tools, regenerate/rebuild, start the primary diagnostic session, and take repeated debugger snapshots of `STM_0.CR`, `STM_0.CNT`, NVIC/clock observations, STM interrupt counters, and matching TaskA execution counters. |
| Expected result/tolerance | If the halted multicore debug state is asserting STM debug freeze, clearing only `FRZ` must allow `STM_0.CNT` and all three STM-driven tasks to progress without changing the clock/MC_ME observations. |
| Actual result | `STM_0.CR` changed from `0x2F03` to `0x2F01` (`TEN=1`, `FRZ=0`). Two snapshots showed `STM_0.CNT` increasing from `17660025` to `36236283`; a later snapshot showed `101678153`. The 10/100/1000 ms interrupt counts were `256/26/3`, exactly matching TaskA execution counts `256/26/3`. NVIC enable and the observed clock/MC_ME fields remained unchanged. |
| Outcome | `PASS` for isolating the apparent task stoppage to STM debug freeze under the primary-only debugger state; no standalone firmware clock or scheduler defect was demonstrated |
| Evidence paths | Debugger screenshots supplied during the session; durable copies and hashes were not archived |
| Deviations/issues | `StmFreezeEnable=false` was diagnostic only. A primary-only launch is not accepted as a normal AMP test because its result depends on debugger treatment of the unlaunched core. |
| Follow-up | Restore `StmFreezeEnable=true`, use the multicore Launch Group for interactive debugging, and implement standalone CM7_2 release through Power/MC_ME. |

### RUN-20261025-014 - PEmicro S32K358 macro interference isolation

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-25 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | PEmicro path from S32DS; exact probe firmware not yet recorded |
| S32DS/PEmicro version | S32DS 3.6.5; `com.pemicro.debug.gdbjtag.pne_6.2.7.202610202056` |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH`; two core-specific child launches coordinated by the FLASH Launch Group |
| Git commit/build ID | Uncommitted MC_ME/TaskB working tree; checkpoint pending |
| Core(s) | `M7_0_0` and `M7_0_2` |
| Test ID(s) | `H1-PEMICRO-MACRO-ISOLATION` |
| Preconditions | The application configures CM7_2 boot address/release through generated Power/MC_ME data; the stock PEmicro S32K358 macro was being executed during debugger startup and CM7_2 did not start at the application vector table. |
| Procedure/command | Inspect the installed PEmicro device macro, rename `S32K358.mac` to `S32K358_.mac` so PEmicro cannot execute it, then repeat the multicore FLASH debug launch. The operator followed the NXP S32K324 dual-core PEmicro guide as precedent. |
| Expected result/tolerance | PEmicro no longer overwrites application-owned MC_ME boot/control state; CM7_0 releases CM7_2 at the linked flash vector address and CM7_2 enters its application path. |
| Actual result | After the macro was disabled, application-owned multicore startup worked. Static inspection confirmed that the macro wrote temporary SRAM vectors, programmed both CM7_0 and CM7_2 MC_ME boot/control registers, and reset both cores. The inspected renamed macro SHA-256 is `8D4092F0348C237A7E5306F972BC9F5F205B3E7BD6DA3F2DBEC0A49C8E642732`. During the subsequent TaskB integration run, CM7_0 faulted while initializing the mailbox; inspection also showed that the disabled macro had previously performed DMA-based SRAM initialization and the firmware startup ECC range stopped at `0x204BFF00`, immediately before the mailbox. |
| Outcome | `PASS` for isolating and proving PEmicro MC_ME interference; `OPEN` for debugger-independent SRAM/ECC initialization and the complete TaskB run |
| Evidence paths | Debugger screenshots supplied during the session; durable copies and hashes were not archived. Installed renamed macro: `C:/NXP/S32DS.3.6.5/eclipse/plugins/com.pemicro.debug.gdbjtag.pne_6.2.7.202610202056/supportFiles_ARM/NXP/S32K3xx/S32K358_.mac`. NXP source: `https://community.nxp.com/t5/S32-Design-Studio-Knowledge-Base/Dual-cores-debugging-with-PEMicro-s-MULTILINK-by-S32DS-for/ta-p/1478645`. |
| Deviations/issues | Renaming an installed plug-in file is host-local and may be undone by update/reinstallation. The complete stock macro cannot remain the hidden owner of SRAM initialization. The current CM7_2 static review also found a missing `Stm_Ip_Init()` call, `STM2_IRQn` generated with `undefined_handler` instead of `STM_2_ISR`, and a `void Core2_Main()` returning application fault values. These are not the cause of the earlier CM7_0 mailbox ECC fault but must be corrected before TaskB qualification. |
| Follow-up | Extend CM7_0 startup shared-SRAM ECC initialization through `0x204C0000`, rebuild both images, then correct the three CM7_2 integration findings and run one bounded dual-core TaskA/TaskB hardware test. |
| Resolution | Completed by `RUN-20261025-015`; the corrected implementation is checkpointed by commit `49b4ad7`. The host-local PEmicro macro workaround remains required for this installed plug-in version. |

### RUN-20261025-015 - Simultaneous CM7_0 TaskA and CM7_2 TaskB execution

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-25 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | PEmicro path from S32DS; exact probe firmware not yet recorded |
| S32DS/PEmicro version | S32DS 3.6.5; `com.pemicro.debug.gdbjtag.pne_6.2.7.202610202056`; stock `S32K358.mac` disabled by local rename |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0, one independent single-core instance per ELF |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH`; multicore FLASH Launch Group |
| Git commit/build ID | Tested pre-commit working tree subsequently checkpointed without further firmware changes as `49b4ad7` (`feat: validate dual-core AMP FreeRTOS runtime`) |
| Core(s) | `M7_0_0` and `M7_0_2` executing simultaneously |
| Clock configuration | STM0 and STM2 configured as independent 1 MHz application timebases with 10/100/1000 ms channels |
| Cache/MPU state | Startup MPU and instruction/data caches enabled; boot mailbox in shareable non-cacheable SRAM |
| Test ID(s) | `H1-DUAL-CORE-TASKA-TASKB` |
| Preconditions | CM7_0 startup shared-SRAM ECC range extended through mailbox end `0x204C0000`; STM2 instance initialized on CM7_2; `STM2_IRQn` generated with `STM_2_ISR`; CM7_2 application entry returns `AppFaultCodeType`; stock PEmicro S32K358 macro disabled. |
| Procedure/command | Clean/rebuild both `Debug_FLASH` images, launch the two child sessions through the multicore FLASH Launch Group, resume execution, suspend for observation, and compare each STM interrupt count with its corresponding task execution count in the owning ELF context. |
| Expected result/tolerance | Both schedulers execute concurrently with fatal state zero. Each 10/100/1000 ms interrupt count equals its corresponding TaskA or TaskB execution count, allowing a transient difference of one if halted between ISR notification and task execution. CM7_2 must report `BOOT_SYNC_WAIT_READY` and enter `Core2_Main()`. |
| Actual result | CM7_2 reported `g_Core2FatalError=0`, `g_Core2MainEntered=1`, and `g_Core2BootWaitStatus=BOOT_SYNC_WAIT_READY`. Its STM2 interrupt counts were `242/24/2` and TaskB execution counts were exactly `242/24/2`. CM7_0 reported `g_Core0FatalError=0`, non-null handles for all three TaskA tasks, STM0 interrupt counts `1638/163/16`, and exactly matching TaskA execution counts `1638/163/16`; its Idle counter was `108529992`. CM7_2 Idle remained zero because its generated `FreeRTOSConfig.h` has `configUSE_IDLE_HOOK=0`, unlike CM7_0 where it is enabled; this is a configuration observation, not a scheduler failure. |
| Outcome | `PASS` for simultaneous dual-image AMP scheduler execution, TaskA/TaskB periodic release, mailbox readiness consumption, application-owned MC_ME release under debug, and debugger-independent mailbox ECC initialization |
| Evidence paths | Debugger screenshots supplied during the session; durable copies and hashes were not archived |
| Deviations/issues | A clean-build console log and exact ELF/map identities were not archived. The run used the debugger Launch Group, so standalone power-cycle/cold-boot release without debugger participation remains unconfirmed. `CORE2_READY` publication, failure/timeout/warm-reset cases, and hardware-assisted wake-up remain open. |
| Follow-up | Git checkpoint `49b4ad7` was created and pushed. Next perform a standalone power-cycle test that flashes both images once and observes execution without allowing the debugger to create or release CM7_2 state; separately implement and prove CM7_2 `WAITING`/`READY`/`FAILED` publication. |

### RUN-20261025-016 - Common-source AMP refactor regression

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-25 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | PEmicro path from S32DS; exact probe firmware not yet recorded |
| S32DS/PEmicro version | S32DS 3.6.5; stock `S32K358.mac` remains disabled by local rename |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0, one independent single-core instance per ELF |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH`; two AMP ELF images observed in their respective debugger contexts |
| Git commit/build ID | Uncommitted common-source/protocol-V3 refactor working tree |
| Core(s) | CPU2 / `M7_0_2` and CPU1 / `M7_0_0` |
| Test ID(s) | `H1-COMMON-SOURCE-AMP-REGRESSION` |
| Preconditions | Both core projects compile the linked `common_src` folder; common core/timebase/status/fault APIs replace the duplicated implementations; TaskA and TaskB definitions and RTD binding descriptors remain image-local. |
| Procedure/command | Build and run the refactored `Debug_FLASH` AMP images on hardware, suspend each debugger context, and compare `g_BspTimebaseInterruptCount[]` with `g_AppTaskDiagnostics[].executionCount` for all three rate indexes. |
| Expected result/tolerance | Each local 10/100/1000 ms STM interrupt count equals the corresponding task execution count, allowing a transient difference of one if halted between notification and execution; task diagnostic `lastError` remains zero. |
| Actual result | CPU2 reported `g_Core2MainEntered=1`, STM counts `168/16/1`, matching TaskB execution counts `168/16/1`, and all three `lastError` fields zero. CPU1 reported STM counts `1444/144/14`, matching TaskA execution counts `1444/144/14`, and all three `lastError` fields zero. The distinct array addresses in the two ELF contexts confirm that the common sources were compiled independently into each AMP image rather than creating shared scheduler/task storage. |
| Outcome | `PASS` for the common-source BSP/timebase/task-diagnostics refactor and simultaneous three-rate task execution on both application cores |
| Evidence paths | User-supplied debugger screenshots `codex-clipboard-6a9409a3-adeb-4bdb-80f7-bc2e69fa4c93.png` (CPU2) and `codex-clipboard-91e73ca8-0565-4b82-aaeb-b0a48a741bc3.png` (CPU1); durable repository copies and hashes not archived |
| Deviations/issues | The screenshots do not expose `g_AppSystemStatus`, so protocol-V3 `STARTING/READY/FAILED` publication is not directly proven by this run. Build console, ELF/map identities, reset/power-cycle behavior, soak duration, and `g_AppIdleIterationCount` were not captured. |
| Follow-up | Inspect `g_AppSystemStatus` from both debugger sessions in one bounded run, then checkpoint the refactor and its documentation. |

### RUN-20261025-017 - Protocol-V3 status confirmation after common-source refactor

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-25 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | PEmicro path from S32DS; exact probe firmware not yet recorded |
| S32DS/PEmicro version | S32DS 3.6.5; stock `S32K358.mac` remains disabled by local rename |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| FreeRTOS version | V11.1.0, one independent single-core instance per ELF |
| Build configuration | `Debug_FLASH`; two AMP ELF images observed in their respective debugger contexts |
| Git commit/build ID | `2ce2e7e` (`Refactor AMP runtime onto common support layer`) |
| Core(s) | CPU2 / `M7_0_2` and CPU1 / `M7_0_0` |
| Test ID(s) | `H1-STATUS-V3-READY` |
| Procedure/command | Observe `g_AppSystemStatus` and the generic rate-indexed diagnostics while both AMP images execute. |
| Expected result/tolerance | Magic `0x41535453`, protocol version `3`, both core states `READY` (`2`), both fatal-error fields `0`; each task count continues to match its STM interrupt count. |
| Actual result | `g_AppSystemStatus.magic=1095980115` (`0x41535453`), `protocolVersion=3`, `core[0].state=2`, `core[0].fatalError=0`, `core[1].state=2`, `core[1].fatalError=0`. CPU2 counts were `389/38/3`, exactly matching its task diagnostics; CPU1 counts were `2506/250/25`, exactly matching its task diagnostics; all observed `lastError` fields were `0`. |
| Outcome | `PASS` for protocol-V3 status publication and dual-core READY/error-free runtime observation |
| Evidence paths | User-supplied debugger screenshots `codex-clipboard-64bb2cd1-611b-4769-b599-6081f57cda8f.png` (CPU2 plus shared status) and `codex-clipboard-2cc33fb9-a2fc-465a-9466-c9b900fb934a.png` (CPU1); durable repository copies and hashes not archived |
| Deviations/issues | Cold power-cycle, standalone no-debugger execution, soak duration, build console, and exact ELF/map identities remain unarchived. |
| Follow-up | Completed: commit `2ce2e7e` pushed to `origin/main`. Retain standalone cold-boot and QEMU parity as subsequent gates. |

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
| H1A Primary FreeRTOS/TaskA runtime | FUNCTIONAL PASS | RUN-20261025-017 | The common-source refactor preserves all three TaskA rates with exact ISR/task-count equality; earlier runs additionally prove the CM7_0 Idle hook. Immediate preemption latency remains unmeasured. |
| H1B Secondary FreeRTOS/TaskB runtime | FUNCTIONAL PASS IN INTEGRATED RUN | RUN-20261025-017 | The common-source refactor preserves CM7_2 entry and all three TaskB rates with exact ISR/task-count equality while both images execute. |
| H1C Simultaneous AMP runtime | FUNCTIONAL PASS | RUN-20261025-017 | Both independently linked common implementations and all six periodic tasks execute with exact ISR/task-count agreement; exact ELF/map artifacts were not archived for a formal overlap check. |
| H1D Observability completion | IN PROGRESS | RUN-20261025-017 | Generic diagnostics and protocol-V3 magic/version/READY/error fields pass on both cores. CM7_2 Idle-hook parity, distinct LED/trace behavior, and the core-0 UART identity banner remain open. |
| H1/2 Combined gate | IN PROGRESS | RUN-20261025-017 | Standalone no-debugger cold boot, defined soak/repetition evidence, negative boot/hook tests, UART/LED observability, and durable qualification artifacts remain open. |
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
