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

Use the same conceptual task set in each image, with core identity supplied by build configuration rather than duplicated behavior hidden in task names.

| Task role | Purpose | Initial state |
|---|---|---|
| Supervisor | startup state, health checks, failure-hook reporting, worker control | periodic or event-driven |
| Heartbeat | prove the local scheduler/tick and provide a core-distinct observable pattern | periodic via `vTaskDelayUntil()` |
| Test worker | execute one bounded peripheral test at a time | blocked on direct notification or queue |

Recommended debugger-visible names are `C0_Supervisor`, `C0_Heartbeat`, `C0_TestWorker`, and the corresponding `C2_...` names. Application-facing creation functions follow the project naming rules, for example `AppTasks_Init()`, while private task entry functions remain file-static, such as `heartbeatTask()`.

Prefer static allocation. Suggested file-static storage names are `s_heartbeatTaskTcb`, `s_heartbeatTaskStack`, and equivalent names for other tasks. Period configuration uses explicit units such as `APP_HEARTBEAT_PERIOD_MS` and conversion through `pdMS_TO_TICKS()`.

## Bring-up sub-gates

| Gate | Scope | Required hardware evidence |
|---|---|---|
| H1A | FreeRTOS on `M7_0_0` only | scheduler starts; one periodic counter advances; no assert/reset |
| H1B | FreeRTOS on `M7_0_2` only | independent scheduler and periodic counter advance |
| H1C | both AMP images together | both counters/ticks advance; memory maps remain isolated; group debug succeeds |
| H1D | reusable task model and observability | supervisor/heartbeat/worker tasks on both cores; distinct LED/trace behavior; core-0 UART identity banner |

The combined H1/2 gate is passed only after every sub-gate passes on the physical board and its evidence is recorded.

## Deferred until the local schedulers are proven

- cross-core test dispatch;
- MU/SEMA42/IPCF selection;
- shared-memory queues and cache-coherency behavior;
- concurrent access to a shared peripheral;
- fault injection and restart of only one application core.
