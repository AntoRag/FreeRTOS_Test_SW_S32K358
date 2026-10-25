# STM test software

Introduced in `v0.2.0`. Functional hardware checks `STM-001..004` have passed on both application cores (`RUN-20261025-018/019/020` in [TEST_LOG.md](../TEST_LOG.md)). Complete timer coverage and exact-value QEMU parity remain open. This guide describes the current implementation and debugger interface.

## Resources and configuration

| Image | Periodic-task releases | Dedicated test resource | Generated channel configuration |
|---|---|---|---|
| `M7_0_0` / CM7_0 | TaskA, STM0_CH0/CH1/CH2 | STM0_CH3 | `Core0StmTestChannel` |
| `M7_0_2` / CM7_2 | TaskB, STM2_CH0/CH1/CH2 | STM2_CH3 | `Core2StmTestChannel` |

Each STM counter is configured at 1 MHz. Task-release periods are 10,000/100,000/1,000,000 ticks for the 10/100/1000 ms tasks. The CH3 test compare interval is 20,000 ticks. `bsp_platform_config.c` in each image binds the common runner to its generated STM instance/channel descriptor; the common runner uses RTD `Stm_Ip` APIs/types.

`BspTimebase_Init()` initializes the owning STM and task channels. `StmTest_Init()` then initializes CH3 before task creation. The existing `TaskA_10ms` or `TaskB_10ms` advances `StmTest_Task10ms()` after consuming a notification; there is no additional test task. The CH3 callback calls `StmTest_NotifyFromISR()` to record events.

The runner copies the generated CH3 descriptor to select one-shot or continuous mode at runtime. It does not reinitialize the STM instance or modify channels 0-2. Change generated configuration through `.mex` and regenerate rather than editing generated files. Both current instances enable debug freeze; see [the build/debug guide](../BUILD_AND_DEBUG.md).

## Command and result interface

Each ELF has its own `g_StmTestCommand` and `g_StmTestResult`. Select the owning core's ELF/debug session before inspecting or changing them.

| Command value | Stable ID | Enum value | Functional pass criterion |
|---|---|---|---|
| `1` | `STM-001` | `STM_TEST_ID_ONESHOT_COMPARE_IRQ` | Exactly one callback, then three further 10 ms activations without another callback. |
| `2` | `STM-002` | `STM_TEST_ID_CONTINUOUS_COMPARE_IRQ` | At least three callbacks during ten 10 ms task activations; the channel is disabled at completion. |
| `3` | `STM-003` | `STM_TEST_ID_COUNTER_MONOTONIC` | Four consecutive samples produce nonzero unsigned modular deltas. This checks progression, not a specified exact delta or full wraparound behavior. |
| `4` | `STM-004` | `STM_TEST_ID_COMPARE_READBACK` | Readback yields `0 < compareRemainingTicks <= 20000`; the channel is then disabled. |

`0` (`STM_TEST_ID_NONE`) means idle. `5` is currently `STM_TEST_ID_COUNT`, an enum sentinel rather than a runnable test; it is rejected with FAIL and `STM_TEST_ERROR_INVALID_ID` (`1`). Test IDs retain their assigned meaning; future tests are appended.

| Result field | Meaning |
|---|---|
| `activeTestId` | Last requested test ID. |
| `status` | `0=IDLE`, `1=RUNNING`, `2=PASS`, `3=FAIL`. |
| `callbackCount` | Observed CH3 callbacks during the running test. |
| `expectedCallbackCount` | Exactly one for STM-001; a minimum of three for STM-002; zero for the non-callback checks. |
| `lastCounterDeltaTicks` | Last sampled unsigned counter delta for STM-003. |
| `compareRemainingTicks` | Compare-minus-counter readback sample for STM-004. |
| `error` | `0=NONE`, `1=INVALID_ID`, `2=CHANNEL_INIT`, `3=CALLBACK_TIMEOUT`, `4=CALLBACK_COUNT`, `5=COUNTER_STALLED`, `6=COMPARE_RANGE`. |

A request resets the result fields. Completion retains the result and clears the command to `0`. The result is replaced when the next request starts. Do not change the command while a test is running; queued requests and abort are not implemented.

## Running a test

1. Build both `Debug_FLASH` images and start the accepted multicore debug workflow described in [BUILD_AND_DEBUG.md](../BUILD_AND_DEBUG.md).
2. Confirm the local periodic timebase/task counts advance before triggering a test. `g_BspTimebaseInterruptCount[0..2]` and `g_AppTaskDiagnostics[0..2].executionCount` correspond to 10/100/1000 ms. `executionCount` counts task entries before their workloads execute.
3. In one core's debug session, set `g_StmTestCommand` to the desired ID and resume execution. Exercise one core/request at a time for initial reproduction.
4. Allow execution to continue, then inspect the result in the same ELF context. Completion is identified by the cleared command and terminal status. A value-5 FAIL is expected input rejection, rather than an STM failure.
5. Record the test ID, all result fields, core, source/binary identity, and debug-freeze setting in the test log. Confirm the regular task releases still advance.

Tests are bounded in 10 ms task activations: continuous mode observes ten activations, counter progression takes four samples, and the one-shot callback wait has a 100-activation timeout followed by its three-activation repeat check. Compare readback finishes in its starting activation. These are bounds relative to a progressing task/timebase, not independent watchdog deadlines. Debug halts extend wall-clock duration.

## Hardware results and model comparison

The recorded one-shot results contain one callback on each core. Continuous checks recorded four callbacks with a minimum requirement of three. Counter-delta samples were 10,000 on CM7_0 and 9,992 on CM7_2; compare-remaining samples were 19,996 and 19,997 respectively. See the dated test records for exact observations and evidence limits.

Raw samples and continuous callback totals can depend on the sampling phase. They are not yet a whole-structure exact-value parity contract. Under [D-027](../DECISIONS.md#d-027---peripheral-model-parity-uses-matching-test-outputs-not-physical-jitter), the same firmware must produce matching canonical results on board and QEMU; phase-dependent values need a deterministic test design or explicit exclusion from that contract before parity can be claimed. Physical jitter measurement is not a release gate.

Remaining coverage includes wraparound, priority/nesting, notification behavior, and timeout/error paths, as scoped in [the roadmap](../ROADMAP.md#phase-5---timers-interrupts-and-timebase). The current activation-based timeout also cannot detect loss of the very task/timebase that advances the runner. Automated transport, registry, abort, and host-driven execution remain future work.

## Source locations

- [Common runner](../../FreeRTOS_Test_SW/common/src/stm_test.c) and [interface](../../FreeRTOS_Test_SW/common/include/stm_test.h).
- [CM7_0 bindings](../../FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_0/src/bsp_platform_config.c) and [CM7_2 bindings](../../FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_2/src/bsp_platform_config.c).
- [Common timebase](../../FreeRTOS_Test_SW/common/src/bsp_timebase.c).
- [TaskA definitions](../../FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_0/src/app_tasks.c) and [TaskB definitions](../../FreeRTOS_Test_SW/FreeRTOS_Test_SW_M7_0_2/src/app_tasks.c).

The architectural choices are recorded in D-026 and D-027 in [DECISIONS.md](../DECISIONS.md).
