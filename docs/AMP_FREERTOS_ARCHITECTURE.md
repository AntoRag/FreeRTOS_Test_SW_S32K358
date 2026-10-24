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

## Startup and initialization ownership

The AMP system has one boot coordinator but two application-initialization paths. `CM7_0` is the only core allowed to perform system-global initialization. `CM7_2` remains held or waits at a bounded `SYSTEM_READY` barrier and must not touch a clocked peripheral before the global state is ready.

Both ELF images retain the standard generated runtime path through `Reset_Handler` and C `main()`. The project-owned logical entrypoints are `Core0_Boot()` and `Core0_Main()` in `M7_0_0`, and `Core2_BootWait()` and `Core2_Main()` in `M7_0_2`. This is an internal responsibility split inside the two application images, not a separate bootloader/application image architecture.

The target sequence is:

1. `CM7_0` performs reset/startup processing and primary memory initialization.
2. `CM7_0` performs `Bsp_SystemInit()`: system clock/PLL setup, global clock distribution, and other explicitly assigned shared configuration.
3. `CM7_0` performs its `Bsp_CoreInit()`: local vector-table/NVIC configuration and other core-local setup.
4. `CM7_0` initializes its owned `STM_0`, creates the `TaskA` set, publishes `SYSTEM_READY` through the boot mailbox with the required memory ordering, and releases or permits `CM7_2` to proceed.
5. In the standalone path, `CM7_0` configures the `CM7_2` boot address and performs the physical core release through `MC_ME`. A debugger or QEMU launcher may perform the physical release instead, but cannot bypass the logical mailbox check.
6. `CM7_2` validates and observes `SYSTEM_READY`, executes the matching memory barrier, performs its own `Bsp_CoreInit()`, initializes only its assigned STM/peripherals, creates the `TaskB` set, and publishes `CORE2_READY`.
7. Each image starts its own single-core FreeRTOS scheduler and enters independent application execution.

`Clock_Ip_Init()` belongs to the primary-only system path because it programs global clock hardware. `IntCtrl_Ip_Init()` belongs to each core-local path because each Cortex-M7 application core controls its own NVIC/vector context. `Stm_Ip_Init()` is called once by the owner of the selected physical STM instance; another image must not reinitialize that same instance.

During bring-up, the debugger group launch may prepare or release both cores. That convenience is not accepted as the standalone boot protocol: `CM7_2` must still use the explicit readiness barrier so the same ordering works after cold boot and under QEMU.

### Boot mailbox contract

Physical core release and logical readiness are separate mechanisms. `MC_ME` owns standalone `CM7_2` start-address programming and hardware release. A persistent boot mailbox in shared SRAM owns `SYSTEM_READY`, `CORE2_READY`, and startup-failure state. The hardware-proven first implementation uses bounded active polling as a temporary bootstrap mechanism. Phase 11 will evaluate an `MSCM` directed interrupt or `MU` event so the waiting core can sleep or perform other controlled work instead of spinning; that event is only a wake-up notification and never replaces mailbox validation.

The mailbox is not ordinary per-image storage. Protocol V1 reserves the same 256-byte range at `0x204BFF00` in all four core-specific linker scripts as the 32-byte-aligned `NOLOAD` section `.boot_mailbox`. It remains outside both images' normal startup copy/zero ranges and lies inside the generated 256 KiB shareable, non-cacheable MPU region beginning at `0x20480000`. Because the primary startup ECC loop also stops before the reserved range, `CM7_0` must explicitly write every mailbox word before publishing a valid state. Its fixed-width 32-bit content includes magic, protocol version, a `CM7_0`-owned system state/error pair, a `CM7_2`-owned core state/error pair, and zero-initialized reserved words. The polling timeout and warm-reset generation handling remain open implementation parameters.

For every published state transition, the producer writes the associated payload, executes `__DMB()`, and then writes the state. After the consumer observes that state, it executes `__DMB()` before reading the associated payload. `volatile` prevents inappropriate compiler treatment of polled fields but does not replace these ordering barriers. Because `__DMB()` does not flush cache lines, changing the mailbox to cacheable memory would require an additional reviewed cache-coherency protocol.

Timeout, invalid magic/version/state, or an explicit peer failure prevents scheduler startup and unowned peripheral access and enters a deterministic diagnostic failure path. None of these runtime behaviors is considered proven until both images clean-build and the complete sequence is exercised on the physical board.

## Initial task model

Each periodic activity is an independent FreeRTOS task. There is no shared cyclic task that executes all 10 ms, 100 ms, or 1000 ms work. Each task owns its own state, stack, activation notification, execution counter, and test responsibility.

| Task class | Scheduling rule | Initial purpose |
|---|---|---|
| 10 ms periodic | blocks until released by the 10 ms STM compare interrupt | short, bounded, time-critical tests |
| 100 ms periodic | blocks until released by the 100 ms STM compare interrupt | medium-rate tests and health sampling |
| 1000 ms periodic | blocks until released by the 1000 ms STM compare interrupt | heartbeat, reporting, and slow tests |
| FreeRTOS Idle task | runs only when no ready application task exists | background counter and minimal non-blocking housekeeping through the Idle hook |

The Idle task is the background task for each RTOS instance; do not create a separate application background task in the baseline. `vApplicationIdleHook()` must never block, wait for a peripheral, take a mutex, or perform an unbounded test. A future background operation that may block or has a deadline must receive its own application task.

The task-family name identifies the owning AMP image. `TaskA` is reserved for `M7_0_0`, while `TaskB` is reserved for `M7_0_2`. Tasks at the same nominal period therefore execute under different, independent FreeRTOS schedulers. Across period classes within each image, use rate-monotonic ordering: 10 ms above 100 ms, and 100 ms above 1000 ms; all application tasks remain above Idle.

The minimum H1D proof set is:

- on `M7_0_0`: `TaskA_10ms`, `TaskA_100ms`, and `TaskA_1000ms`;
- on `M7_0_2`: `TaskB_10ms`, `TaskB_100ms`, and `TaskB_1000ms`;
- each kernel's Idle task with a per-core Idle-hook counter.

This is a six-application-task system in total. Debugger-visible names retain the family and period, for example `TaskA_10ms` in the primary image and `TaskB_10ms` in the secondary image; the ELF/debug session supplies the physical-core context. The configured `configMAX_TASK_NAME_LEN` must be checked before these strings are frozen. Source-level functions use module prefixes and functional names; the A/B identifiers remain stable core-family names even when the task workload later receives a purpose-specific test name.

Prefer static allocation. Each task has separate file-static TCB and stack storage, for example `s_taskA10msTcb` and `s_taskA10msStack`. Period constants include units, such as `APP_TASK_PERIOD_10_MS`; STM compare values are derived from the configured STM clock, not from FreeRTOS ticks. Initial instrumentation maintains at least activation and completed-execution counters; detailed deadline and WCET policy is deferred. Measurements from the two independent STM counters are not assumed to be phase-aligned across cores.

## STM activation path

Each core owns one STM instance selected and validated through S32 Configuration Tools. Three continuous compare channels are reserved on that instance: one channel each for 10 ms, 100 ms, and 1000 ms. The exact `STM_n` instances, channel numbers, clocks, and interrupt routes are frozen only after configuration validation confirms per-core ownership.

For each compare event, the RTD callback/ISR services the STM event and releases the single corresponding task in its local image using a FreeRTOS task-notification API permitted from ISR context. It then requests a context switch through the Cortex-M7 FreeRTOS port when the task was awakened at a higher priority. The required configuration invariant is `configUSE_PREEMPTION = 1`.

Each periodic task spends its inactive time blocked on its own notification; it does not call `vTaskDelay()`, `vTaskDelayUntil()`, poll a counter, or busy-wait for release. The FreeRTOS kernel tick remains available for kernel scheduling and timeouts, but it is not the application cyclic-task activation source.

An activation received while a task is still executing cannot create a second concurrent invocation of that task. The notification can record a pending release, but the policy for coalescing, queuing, skipping, or faulting on overruns is deliberately deferred. Initial instrumentation records activation and execution counts so overruns are observable without yet defining their product reaction.

Every STM interrupt that calls a FreeRTOS `...FromISR` API must use an interrupt priority allowed by the port's maximum-syscall-priority configuration.

## Bring-up sub-gates

| Gate | Scope | Required hardware evidence |
|---|---|---|
| H1A | FreeRTOS on `M7_0_0` only | scheduler starts; one STM interrupt releases a blocked task; preemption occurs; no assert/reset |
| H1B | FreeRTOS on `M7_0_2` only | independent STM interrupt, scheduler, and released-task counter advance |
| H1C | both AMP images together | both counters/ticks advance; memory maps remain isolated; group debug succeeds |
| H1D | reusable task model and observability | STM-released A-family tasks at 10/100/1000 ms on `M7_0_0`, B-family tasks at the same rates on `M7_0_2`, and Idle-hook background activity on both cores; distinct LED/trace behavior; core-0 UART identity banner |

The combined H1/2 gate is passed only after every sub-gate passes on the physical board and its evidence is recorded.

## Deferred until the local schedulers are proven

- cross-core test dispatch;
- MU/SEMA42/IPCF selection;
- shared-memory queues and cache-coherency behavior;
- concurrent access to a shared peripheral;
- fault injection and restart of only one application core.
