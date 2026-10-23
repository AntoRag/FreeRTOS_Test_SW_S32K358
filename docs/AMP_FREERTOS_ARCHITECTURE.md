# AMP FreeRTOS Architecture

Status: accepted architectural baseline as of 2026-10-23.

## Execution model

The software uses asymmetric multiprocessing (AMP), not FreeRTOS SMP.

| Physical execution resource | Project/image | RTOS instance | Initial responsibility |
|---|---|---|---|
| `CM7_0` with lockstep checker `CM7_1` | `FreeRTOS_Test_SW_M7_0_0` | Independent single-core FreeRTOS | boot policy, global clocks, system initialization, control/telemetry, primary tests |
| `CM7_2` | `FreeRTOS_Test_SW_M7_0_2` | Independent single-core FreeRTOS | explicitly assigned test workers and peripherals |
| `CM7_1` checker | none | none | hardware lockstep checking only; never hosts code or tasks |

Each image has its own `FreeRTOSConfig.h`, scheduler, tick, task lists, kernel objects, stacks, heap/static memory, exception context, and linker-owned RAM. The required invariant in both configurations is:

```text
configUSE_SMP = 0
configNUMBER_OF_CORES = 1
```

## Grounded integration reference

The H0 project is currently bare-metal even though its name contains `FreeRTOS`.

- Both `.mex` files select `OsIfBaremetalType`.
- Both generated `OsIf_Cfg.h` files define `USING_OS_BAREMETAL`.
- No `FreeRTOSConfig.h`, kernel source, or application task API call exists in either project.

The installed S32DS 3.6.5 PlatformSDK provides:

- FreeRTOS kernel V11.1.0;
- GCC Cortex-M7 port `portable/GCC/ARM_CM7/r0p1`;
- `FreeRTOS_Toggle_Led_Example_S32K358`, which is the primary reference for per-image SDK components, OSIF selection, generated configuration, interrupt setup, and build paths;
- an S32K358 SMP example that is not part of the selected architecture.

Do not copy kernel files or invent port settings manually. Add the matching SDK components through S32DS/S32 Configuration Tools, regenerate, inspect the diff, and build one project at a time.

## Ownership rules

1. `M7_0_0` is the only owner of global clock and system initialization.
2. Every peripheral has one owning core at a time. Ownership is recorded before its first test.
3. `M7_0_2` must not assume a shared/global service is ready until a bounded startup handshake says so.
4. A peripheral driver instance, interrupt route, DMA channel, and associated buffers belong to the same owner unless an explicit reviewed design says otherwise.
5. Shared RAM is not ordinary shared state. Its layout, cache attributes, barriers, integrity fields, and producer/consumer protocol must be specified before use.
6. FreeMASTER, if retained, is initially hosted only by `M7_0_0`; two cores must not contend for one transport.

## Initial task model

Each periodic activity is an independent FreeRTOS task. There is no shared cyclic dispatcher for all 10 ms, 100 ms, or 1000 ms work. Each task owns its own state, stack, timing loop, execution counter, and test responsibility.

| Task class | Scheduling rule | Initial purpose |
|---|---|---|
| 10 ms periodic | independent task loop using `vTaskDelayUntil()` | short, bounded, time-critical tests |
| 100 ms periodic | independent task loop using `vTaskDelayUntil()` | medium-rate tests and health sampling |
| 1000 ms periodic | independent task loop using `vTaskDelayUntil()` | heartbeat, reporting, and slow tests |
| FreeRTOS Idle task | runs only when no ready application task exists | background counter and minimal non-blocking housekeeping through the Idle hook |

The Idle task is the background task for each RTOS instance; do not create a separate application background task in the baseline. `vApplicationIdleHook()` must never block, wait for a peripheral, take a mutex, or perform an unbounded test. A future background operation that may block or has a deadline must receive its own application task.

The task model supports more than one independent task at the same period. For example, `TaskA_10ms` and `TaskB_10ms` have separate entry functions and `vTaskDelayUntil()` state. Tasks in the same period class have equal priority and must not depend on their relative execution order. Across period classes, use rate-monotonic ordering: 10 ms above 100 ms, and 100 ms above 1000 ms; all application tasks remain above Idle.

The minimum H1D proof set on each core is:

- `TaskA_10ms` and `TaskB_10ms`, proving that same-rate activities are independent;
- `TaskA_100ms`;
- `TaskA_1000ms`;
- the kernel Idle task with a per-core Idle-hook counter.

Debugger-visible task names include physical core identity, for example `C0_TaskA_10ms`, `C0_TaskB_10ms`, and the corresponding `C2_...` names. The configured `configMAX_TASK_NAME_LEN` must be checked before these strings are frozen. Source-level functions use module prefixes and functional names; generic `TaskA`/`TaskB` identifiers are bring-up names and are replaced by purpose-specific names when a task receives a permanent peripheral-test role.

Prefer static allocation. Each task has separate file-static TCB and stack storage, for example `s_taskA10msTcb` and `s_taskA10msStack`. Period constants include units, such as `APP_TASK_PERIOD_10_MS`, and are converted using `pdMS_TO_TICKS()`. Each task maintains at least a release counter, deadline-miss counter, last execution time, and maximum execution time. Measurements from the two independent RTOS ticks are not assumed to be phase-aligned across cores.

## Bring-up sub-gates

| Gate | Scope | Required hardware evidence |
|---|---|---|
| H1A | FreeRTOS on `M7_0_0` only | scheduler starts; one periodic counter advances; no assert/reset |
| H1B | FreeRTOS on `M7_0_2` only | independent scheduler and periodic counter advance |
| H1C | both AMP images together | both counters/ticks advance; memory maps remain isolated; group debug succeeds |
| H1D | reusable task model and observability | independent 10/100/1000 ms tasks plus Idle-hook background activity on both cores; distinct LED/trace behavior; core-0 UART identity banner |

The combined H1/2 gate is passed only after every sub-gate passes on the physical board and its evidence is recorded.

## Deferred until the local schedulers are proven

- cross-core test dispatch;
- MU/SEMA42/IPCF selection;
- shared-memory queues and cache-coherency behavior;
- concurrent access to a shared peripheral;
- fault injection and restart of only one application core.
