# AMP FreeRTOS Architecture

Status: accepted architectural baseline; implementation status updated 2026-10-25.

## Execution model

The software uses asymmetric multiprocessing (AMP), not FreeRTOS SMP.

| Physical execution resource | Project/image | RTOS instance | Initial responsibility |
|---|---|---|---|
| `CM7_0` with lockstep checker `CM7_1` | `FreeRTOS_Test_SW_M7_0_0` | Independent single-core FreeRTOS | boot policy, global clocks, system initialization, control/telemetry, primary tests |
| `CM7_2` | `FreeRTOS_Test_SW_M7_0_2` | Independent single-core FreeRTOS | explicitly assigned test workers and peripherals |
| `CM7_1` checker | none | none | hardware lockstep checking only; never hosts code or tasks |

Each image has its own `FreeRTOSConfig.h`, scheduler, tick, task lists, kernel objects, stacks, heap/static memory, exception context, and linker-owned RAM. The required invariant in both configurations is:

```text
configNUMBER_OF_CORES = 1
```

The installed generated configuration does not emit a `configUSE_SMP` macro; single-core operation is represented by `configNUMBER_OF_CORES = 1` in each image.

## Grounded integration reference and current baseline

H0 was the historical bare-metal baseline. The current projects are no longer bare-metal:

- both `.mex` configurations select the FreeRTOS OS interface;
- both images contain independent FreeRTOS V11.1.0 kernels and generated `FreeRTOSConfig.h` files;
- both configurations use `configNUMBER_OF_CORES = 1`, and SMP remains disabled;
- `M7_0_0` executes the STM0-released TaskA set and `M7_0_2` executes the STM2-released TaskB set;
- `RUN-20261025-015` proves simultaneous hardware execution with exact interrupt-to-task count equality and fatal states zero;
- commit `49b4ad7` is the checkpoint for that proven runtime baseline.

The installed S32DS 3.6.5 PlatformSDK provides:

- FreeRTOS kernel V11.1.0;
- GCC Cortex-M7 port `portable/GCC/ARM_CM7/r0p1`;
- `FreeRTOS_Toggle_Led_Example_S32K358`, which is the primary reference for per-image SDK components, OSIF selection, generated configuration, interrupt setup, and build paths;
- an S32K358 SMP example that is not part of the selected architecture.

Do not invent port settings manually or edit generated configuration directly. Component/configuration changes go through S32DS/S32 Configuration Tools, followed by regeneration, diff review, and per-project build.

## Ownership rules

1. `M7_0_0` is the only owner of global clock and system initialization.
2. Every peripheral has one owning core at a time. Ownership is recorded before its first test.
3. `M7_0_2` must not assume a shared/global service is ready until a bounded startup handshake says so.
4. A peripheral driver instance, interrupt route, DMA channel, and associated buffers belong to the same owner unless an explicit reviewed design says otherwise.
5. Shared RAM is not ordinary shared state. Its layout, cache attributes, barriers, integrity fields, and producer/consumer protocol must be specified before use.
6. FreeMASTER, if retained, is initially hosted only by `M7_0_0`; two cores must not contend for one transport.

## Startup and initialization ownership

The AMP system has one boot coordinator but two application-initialization paths. `CM7_0` is the only core allowed to perform system-global initialization. `CM7_2` remains held or waits at a bounded `SYSTEM_READY` barrier and must not touch a clocked peripheral before the global state is ready.

Both ELF images retain the standard generated runtime path through `Reset_Handler` and C `main()`. The project-owned logical entrypoints are `Core0_Boot()` and `Core0_Main()` in `M7_0_0`, and `Core2_Boot()` and `Core2_Main()` in `M7_0_2`. This is an internal responsibility split inside the two application images, not a separate bootloader/application image architecture.

The implemented sequence is:

1. `CM7_0` performs reset/startup processing, including 64-bit ECC initialization of the complete shared-SRAM range through `0x204C0000`.
2. `Core0_Boot()` initializes boot-mailbox V1 and calls `Bsp_SystemInit()`, which initializes the Power IP support and the global clock tree.
3. `CM7_0` publishes `SYSTEM_READY` with the producer barrier and invokes `Power_Ip_SetMode()` to apply the generated MC_ME boot-address/core-clock configuration for CM7_2.
4. `Core0_Main()` performs CM7_0-local interrupt initialization, initializes STM0, creates TaskA, starts its three channels, and starts the primary scheduler.
5. `Core2_Boot()` waits with a bounded poll count, validates `SYSTEM_READY`, and executes the consumer barrier.
6. `Core2_Main()` performs CM7_2-local interrupt initialization, initializes STM2, creates TaskB, starts its three channels, and starts the secondary scheduler.

The protocol target additionally requires CM7_2 to publish `WAITING`, `READY`, or `FAILED` in its single-writer fields. Those writes and a bounded CM7_0 acknowledgement wait are not implemented yet.

`Clock_Ip_Init()` belongs to the primary-only system path because it programs global clock hardware. `IntCtrl_Ip_Init()` belongs to each core-local path because each Cortex-M7 application core controls its own NVIC/vector context. `Stm_Ip_Init()` is called once by the owner of the selected physical STM instance; another image must not reinitialize that same instance.

During bring-up, the debugger group launch may prepare or release both cores. That convenience is not accepted as the standalone boot protocol: `CM7_2` must still use the explicit readiness barrier so the same ordering works after cold boot and under QEMU.

For normal interactive hardware debugging, use the multicore Launch Group so both core sessions have an explicit, coordinated debugger state. A primary-only launch that leaves `CM7_2` halted is diagnostic-only: hardware testing showed that an asserted multicore debug state can freeze `STM_0` when `StmFreezeEnable` is set even while `CM7_0` and its FreeRTOS Idle task continue executing. The selected baseline therefore keeps STM debug freeze enabled and coordinates both cores through the Launch Group; debugger-created core release remains separate from the standalone boot design.

The stock PEmicro S32K358 startup macro is not compatible with application-owned MC_ME boot when executed unmodified. Inspection showed that it installs temporary SRAM vectors, rewrites both cores' MC_ME control/boot-address state, and resets both cores. The confirmed local diagnostic workaround is to rename `S32K358.mac` so the debugger cannot execute it, following the analogous NXP S32K324 guidance. Because the macro also initialized SRAM through DMA, firmware startup must independently establish ECC for every RAM region it will use. The CM7_0 startup owns this operation and its existing 64-bit shared-SRAM loop must include the mailbox through address `0x204BFFFF`. No debugger script is part of the deployable boot contract.

The generated `Debug_FLASH` Power configuration currently uses the CM7_2 boot address `0x00700000`. Its provenance is the CM7_2 linker region `int_flash_c2`, whose origin is `0x00700000`, together with the linker relationship `__ENTRY_VTABLE = __INIT_INTERRUPT_START`. `RUN-20261025-015` confirms this application-owned path while using the multicore debugger, but the address remains a proposed standalone value until a no-debugger cold boot proves that MC_ME starts CM7_2 at its vector table. The `0x00800000` literal in the consulted NXP example belongs to that example's different linker layout and is not a project default.

### Boot mailbox contract

Physical core release and logical readiness are separate mechanisms. `MC_ME` owns standalone `CM7_2` start-address programming and hardware release. A persistent boot mailbox in shared SRAM owns `SYSTEM_READY` and reserves CM7_2-owned readiness/failure fields. The hardware-proven first implementation uses bounded active polling as a temporary bootstrap mechanism, but currently publishes only the CM7_0 system state. Phase 11 will evaluate an `MSCM` directed interrupt or `MU` event so the waiting core can sleep or perform other controlled work instead of spinning; that event is only a wake-up notification and never replaces mailbox validation.

The mailbox is not ordinary per-image storage. Protocol V1 reserves the same 256-byte range at `0x204BFF00` in all four core-specific linker scripts as the 32-byte-aligned `NOLOAD` section `.boot_mailbox`. It remains outside both images' normal C-runtime copy/zero ranges and lies inside the generated 256 KiB shareable, non-cacheable MPU region beginning at `0x20480000`. Before C starts, CM7_0's 64-bit shared-SRAM startup loop establishes ECC for the complete range through `0x204BFFFF`; CM7_2 does not repeat that initialization. `BootSync_PrimaryInitialize()` then explicitly writes every protocol word before publishing a valid state. Its fixed-width 32-bit content includes magic, protocol version, a `CM7_0`-owned system state/error pair, a `CM7_2`-owned core state/error pair, and zero-initialized reserved words. The polling timeout and warm-reset generation handling remain open implementation parameters.

For every published state transition, the producer writes the associated payload, executes `__DMB()`, and then writes the state. After the consumer observes that state, it executes `__DMB()` before reading the associated payload. `volatile` prevents inappropriate compiler treatment of polled fields but does not replace these ordering barriers. Because `__DMB()` does not flush cache lines, changing the mailbox to cacheable memory would require an additional reviewed cache-coherency protocol.

Timeout, invalid magic/version/state, or an explicit primary failure prevents CM7_2 scheduler startup and returns a distinct local boot-wait status. These paths are implemented but remain unproven by deliberate negative hardware tests. CM7_2 peer-failure publication and CM7_0 acknowledgement handling are not implemented.

## Initial task model

Each periodic activity is an independent FreeRTOS task. There is no shared cyclic task that executes all 10 ms, 100 ms, or 1000 ms work. Each task owns its own state, stack, activation notification, execution counter, and test responsibility.

| Task class | Scheduling rule | Initial purpose |
|---|---|---|
| 10 ms periodic | blocks until released by the 10 ms STM compare interrupt | short, bounded, time-critical tests |
| 100 ms periodic | blocks until released by the 100 ms STM compare interrupt | medium-rate tests and health sampling |
| 1000 ms periodic | blocks until released by the 1000 ms STM compare interrupt | heartbeat, reporting, and slow tests |
| FreeRTOS Idle task | runs only when no ready application task exists | background counter and minimal non-blocking housekeeping through the Idle hook |

The Idle task is the intended background task for each RTOS instance; do not create a separate application background task in the baseline. `vApplicationIdleHook()` must never block, wait for a peripheral, take a mutex, or perform an unbounded test. CM7_0 currently enables and proves its Idle hook; CM7_2 currently has `configUSE_IDLE_HOOK=0`, so configuration parity remains open. A future background operation that may block or has a deadline must receive its own application task.

The task-family name identifies the owning AMP image. `TaskA` is reserved for `M7_0_0`, while `TaskB` is reserved for `M7_0_2`. Tasks at the same nominal period therefore execute under different, independent FreeRTOS schedulers. Across period classes within each image, use rate-monotonic ordering: 10 ms above 100 ms, and 100 ms above 1000 ms; all application tasks remain above Idle.

The minimum H1D proof set is:

- on `M7_0_0`: `TaskA_10ms`, `TaskA_100ms`, and `TaskA_1000ms`;
- on `M7_0_2`: `TaskB_10ms`, `TaskB_100ms`, and `TaskB_1000ms`;
- each kernel's Idle task with a per-core Idle-hook counter.

The six application tasks are implemented and hardware-proven. The CM7_2 Idle-hook counter is the remaining item in this minimum set. Debugger-visible names retain the family and period, for example `TaskA_10ms` in the primary image and `TaskB_10ms` in the secondary image; the ELF/debug session supplies the physical-core context. Both generated configurations use `configMAX_TASK_NAME_LEN=20`, which accommodates the current names. Source-level functions use module prefixes and functional names; the A/B identifiers remain stable core-family names even when the task workload later receives a purpose-specific test name.

Prefer static allocation. Each task has separate file-static TCB and stack storage, for example `s_taskA10msTcb` and `s_taskA10msStack`. Period constants include units, such as `APP_TASK_PERIOD_10_MS`; STM compare values are derived from the configured STM clock, not from FreeRTOS ticks. Initial instrumentation maintains at least activation and completed-execution counters; detailed deadline and WCET policy is deferred. Measurements from the two independent STM counters are not assumed to be phase-aligned across cores.

## STM activation path

Each core owns one STM instance selected and validated through S32 Configuration Tools. CM7_0 owns STM0 channels 0/1/2 and CM7_2 owns STM2 channels 0/1/2, at configured 1 MHz counter rates for 10/100/1000 ms releases. The corresponding interrupt routes are hardware-proven by `RUN-20261025-015`.

For each compare event, the RTD callback/ISR services the STM event and releases the single corresponding task in its local image using a FreeRTOS task-notification API permitted from ISR context. It then requests a context switch through the Cortex-M7 FreeRTOS port when the task was awakened at a higher priority. The required configuration invariant is `configUSE_PREEMPTION = 1`.

Each periodic task spends its inactive time blocked on its own notification; it does not call `vTaskDelay()`, `vTaskDelayUntil()`, poll a counter, or busy-wait for release. The FreeRTOS kernel tick remains available for kernel scheduling and timeouts, but it is not the application cyclic-task activation source.

An activation received while a task is still executing cannot create a second concurrent invocation of that task. The notification can record a pending release, but the policy for coalescing, queuing, skipping, or faulting on overruns is deliberately deferred. Initial instrumentation records activation and execution counts so overruns are observable without yet defining their product reaction.

Every STM interrupt that calls a FreeRTOS `...FromISR` API must use an interrupt priority allowed by the port's maximum-syscall-priority configuration.

## Bring-up sub-gates

| Gate | Status | Scope | Required hardware evidence |
|---|---|---|---|
| H1A | `FUNCTIONAL PASS` | FreeRTOS on `M7_0_0` | scheduler and STM0-to-TaskA release paths pass; immediate preemption latency is unmeasured |
| H1B | `FUNCTIONAL PASS` in integrated run | FreeRTOS on `M7_0_2` | STM2 interrupt, scheduler, and TaskB counters advance in `RUN-20261025-015`; no secondary-only artifact was retained |
| H1C | `FUNCTIONAL PASS` | both AMP images together | both task sets advance with fatal states zero and group debug succeeds; exact ELF/map artifacts were not archived for a formal overlap check |
| H1D | `IMPLEMENTED`, evidence incomplete | reusable task model and observability | all six STM-released tasks pass; CM7_2 Idle hook, distinct LED/trace behavior, and core-0 UART identity banner remain open |

The combined H1/2 gate is passed only after every sub-gate passes on the physical board and its evidence is recorded.

## Deferred beyond the proven scheduler baseline

- cross-core test dispatch;
- MU/SEMA42/IPCF selection;
- shared-memory queues and cache-coherency behavior;
- concurrent access to a shared peripheral;
- fault injection and restart of only one application core.
