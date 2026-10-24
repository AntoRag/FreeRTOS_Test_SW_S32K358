# Decision Log

Status values: `PROPOSED`, `ACCEPTED`, `SUPERSEDED`, `REJECTED`.

## D-001 - Real hardware is the reference oracle

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Every phase requires physical FRDM-A-S32K358 evidence. QEMU results are compared against hardware behavior and cannot replace it.
- Reason: The software exists to validate peripheral/ASIC models.

## D-002 - Durable, evidence-aware project memory

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Maintain context, roadmap, engineering rules, decisions, and test evidence under `docs/`; future agents read them before acting.
- Reason: Fast, reliable handoff without converting assumptions into facts.

## D-002A - Board health is an accepted prerequisite

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Do not spend project time re-proving basic FRDM-A-S32K358 power-up or generic debugger connectivity; the user has already tested the board successfully.
- Boundary: Each project phase still needs its own real-board functional evidence. A working board does not imply that a new image, RTOS configuration, or peripheral test passes.

## D-003 - Initial execution core

- Status: SUPERSEDED by D-011
- Date: 2026-10-23
- Decision: Run the initial application on the `CM7_0` lockstep pair and keep independent `CM7_2` parked/minimal until Phase 11.
- Reason: Lockstep is required by the project's ASIL-D objective and also reduces boot, memory-map, cache, and debug variables while establishing the test oracle.

## D-004 - Application language

- Status: PROPOSED
- Date: 2026-10-23
- Decision: Use C99 for the test application and adapters; defer C++.
- Reason: aligns with NXP RTD/FreeRTOS examples, minimizes runtime/ABI features, and simplifies QEMU parity.

## D-005 - Allocation policy

- Status: PROPOSED
- Date: 2026-10-23
- Decision: Prefer static allocation for FreeRTOS kernel objects and fixed-capacity test infrastructure. Dynamic allocation is allowed only for a test whose subject is allocation behavior.
- Reason: deterministic resource use and reproducible failure behavior.

## D-006 - Host control channel

- Status: PROPOSED
- Date: 2026-10-23
- Decision: Use LPUART6 via onboard OpenSDA as the initial command/result transport with the versioned pipe-delimited ASCII protocol in `ENGINEERING_RULES.md`.
- Reason: board-default PC connectivity and easy reuse by a QEMU character device.
- Risk: confirm that debug firmware exposes the serial bridge and identify the host COM port.

## D-007 - Test software architecture

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: separate application-task logic, FreeRTOS hooks, board initialization, timer adaptation, target-independent tests, and NXP-generated platform code. Keep `main.c` as orchestration only.
- Initial source modules: `app_tasks.c/.h` owns task functions, handles, creation, release interfaces, and execution counters; `bsp_init.c/.h` adapts clock and interrupt initialization; `bsp_timebase.c/.h` adapts STM initialization, channel control, and callbacks; `freertos_hooks.c` owns the configured application hooks. The exact directory hierarchy may evolve without changing these ownership boundaries.
- Dependency rule: application tasks may depend on FreeRTOS but not on STM or generated NXP configuration symbols. The STM callback layer releases tasks only through the public `app_tasks` ISR interface.
- Refactoring rule: establish this split without changing timing, task priorities, initialization order, counter semantics, or runtime behavior; clean-build and repeat `RUN-20261024-005` before adding another task.
- Reason: preserves test intent across physical hardware, QEMU, and later ASIC models while keeping generated RTD dependencies behind a thin board-support boundary.

## D-008 - Initial project source

- Status: SUPERSEDED by D-012
- Date: 2026-10-23
- Decision: prefer a current NXP example matching S32K358/FRDM-A-S32K358, RTD, FreeRTOS, and available debugger; otherwise create a minimal blank project with those exact live options.
- Reason: a board-tested example reduces startup and linker uncertainty while the user learns S32DS.
- Confirmation needed: installed example catalog and exact package versions.

## D-009 - FreeMASTER role

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Do not treat FreeMASTER as part of FreeRTOS or as a prerequisite for the kernel. It is removed from the active baseline and deferred until a deliberate runtime-observability use case is implemented.
- Current state: the firmware does not initialize or poll FreeMASTER. It is therefore not a current hardware-model dependency.
- Integration boundary: keep the first UART test protocol independent of FreeMASTER; if FreeMASTER is reintroduced, add its exact transport peripheral to `PERIPHERALS_USED.md` only when runtime code actually uses it.

## D-010 - Multicore configuration ownership

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Treat the two per-core `.mex` files as generated views of one MCU configuration. Keep one reviewed system-level source of truth for clocks, pins, peripheral ownership, and initialization order; assign every peripheral and global initialization action to exactly one core.
- Initial ownership: `CM7_0` owns boot, clocks, system/global initialization, and the external control/telemetry channel. `CM7_2` runs an independent FreeRTOS instance and only accesses peripherals explicitly assigned to it. Shared resources require an IPC and synchronization design before use.
- Baseline gate: use one frozen S32DS/S32CT/RTD version, validate and regenerate both `.mex` files with that tool set, review the diff, clean-build both images, and verify the group image on the real board.
- Compliance boundary: S32 Configuration Tools validation and a successful build establish configuration/tool consistency only. ISO 26262/ASIL claims additionally require the applicable NXP safety manual and assumptions of use, safety requirements and traceability, independent review, and recorded verification evidence.

## D-011 - Dual-core AMP FreeRTOS from the first functional phase

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Merge the former bare-metal observability and FreeRTOS-baseline phases. Run one independent, single-core FreeRTOS instance in each existing project/ELF: one on `M7_0_0` and one on `M7_0_2`. Do not use FreeRTOS SMP.
- Configuration invariant: each kernel uses `configUSE_SMP = 0` and `configNUMBER_OF_CORES = 1`. `CM7_1` remains the lockstep checker and never hosts tasks.
- Reason: explicit spatial/temporal ownership, fault containment, independent scheduling, and per-core peripheral responsibility fit the intended automotive validation architecture.
- Integration policy: keep the two-image H0 baseline and introduce FreeRTOS through staged per-core hardware gates before enabling simultaneous peripheral work.

## D-012 - NXP FreeRTOS integration references

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Use the installed NXP `FreeRTOS_Toggle_Led_Example_S32K358` and the ARM Cortex-M7 `r0p1` port as the primary integration references for each AMP image. The installed S32K358 SMP example may be inspected for MCU startup details only; its scheduler architecture is not adopted.
- Installed kernel identity: FreeRTOS `V11.1.0` under `PlatformSDK_S32K3/FreeRTOS` in S32DS 3.6.5.
- Reason: use the exact vendor-supplied port, S32 Configuration Tools component model, interrupt integration, and version that match the installed RTD environment.

## D-013 - Independent per-core periodic task model

- Status: ACCEPTED
- Date: 2026-10-23
- Revised: 2026-10-24
- Decision: Implement one independent periodic task per rate class in each AMP image. The lockstep application image `M7_0_0` owns the `TaskA` family: `TaskA_10ms`, `TaskA_100ms`, and `TaskA_1000ms`. The independent-core image `M7_0_2` owns the `TaskB` family: `TaskB_10ms`, `TaskB_100ms`, and `TaskB_1000ms`. `CM7_1` is the lockstep checker and does not execute another task set. The baseline therefore contains six application tasks total, not six tasks per core.
- Naming invariant: `TaskA` always identifies execution by the `M7_0_0` lockstep application image; `TaskB` always identifies execution by the `M7_0_2` image. A task-family letter must never be reused on the other image.
- Original scheduling policy: tasks were initially to self-release using `vTaskDelayUntil()`. This portion is superseded by D-014; task independence, the three rate classes, and rate-monotonic priority ordering remain accepted.
- Background policy: use the FreeRTOS Idle task and a strictly non-blocking Idle hook as the per-core background activity. Do not create a separate background application task in the baseline.
- Reason: independent task state and timing make interference, starvation, WCET, missed releases, and peripheral-model behavior observable without coupling unrelated tests through one dispatcher.

## D-014 - STM interrupt-driven task activation and preemption

- Status: ACCEPTED
- Date: 2026-10-23
- Decision: Application periodic tasks do not self-release with FreeRTOS delay APIs. On each core, three continuous STM compare channels generate the 10 ms, 100 ms, and 1000 ms activation interrupts. Each channel releases exactly one task in the local image using an ISR-safe FreeRTOS task notification: an A-family task on `M7_0_0` or a B-family task on `M7_0_2`.
- Preemption policy: set `configUSE_PREEMPTION = 1`. After releasing tasks, the ISR requests the FreeRTOS port context switch when required, so a newly readied higher-priority task preempts lower-priority application work.
- Ownership policy: use a core-owned STM instance and route its interrupts to that core. Freeze exact STM instances and channel numbers only after S32 Configuration Tools validates the two AMP configurations and their interrupt ownership.
- Kernel-tick boundary: the FreeRTOS tick remains enabled for kernel services and timeouts but is not the application cyclic-task activation source.
- Overrun boundary: record releases and executions, but defer the policy for accumulated notifications, skipped releases, deadline misses, and fault reaction to a later decision.
- Reason: hardware-timer activation matches the intended automotive cyclic execution model and gives hardware, QEMU, and ASIC models an explicit interrupt-to-preemption behavior to validate.

## D-015 - Staged FreeRTOS MPU integration

- Status: ACCEPTED
- Date: 2026-10-24
- Decision: Establish and hardware-test the scheduler baseline with the FreeRTOS task-level MPU port disabled (`configENABLE_MPU = 0` and `configUSE_MPU_WRAPPERS_V1 = 0`), while retaining the S32K3 startup MPU configuration selected by the project-level `MPU_ENABLE` definition. Add FreeRTOS privileged/unprivileged task isolation as a dedicated gate before expanding to the complete peripheral-test task set.
- MPU gate scope: define the S32K358 AMP-specific linker regions and symbols for privileged functions, system calls, privileged data, application code/data, task stacks, and task-owned RAM/MMIO; add MemManage fault diagnostics; prove permitted and intentionally forbidden accesses on hardware for each core.
- Reason: the NXP FreeRTOS MPU port changes linker layout, task creation, system-call wrappers, and per-task access policy. Separating scheduler bring-up from those changes gives a known-good diagnostic baseline without abandoning MPU isolation as an architectural goal.
- Compliance boundary: hardware MPU enablement, FreeRTOS task isolation, and lockstep are safety mechanisms but do not by themselves establish an ISO 26262 or ASIL claim.

## D-016 - Device-model hardware inventory

- Status: ACCEPTED
- Date: 2026-10-24
- Decision: Maintain `docs/PERIPHERALS_USED.md` as the single living inventory of exact hardware modules required by the current firmware, with one description of how each module is used.
- Inclusion rule: list a module only when executed startup, RTD, kernel, or application code accesses its register block or depends on its interrupt/exception behavior. Installed drivers, included headers, linked-but-unreached code, and configured clock gates alone do not qualify.
- Scope rule: the inventory is the union of both AMP images and does not carry a core-ownership field; a module required by either image is a requirement for the device model.
- Maintenance rule: update the inventory whenever software development introduces or removes a hardware dependency. Real-board and QEMU outcomes remain in `TEST_LOG.md`, not in the inventory.

## D-017 - CM7_0 boot coordination and split initialization ownership

- Status: ACCEPTED
- Date: 2026-10-24
- Decision: `CM7_0` in image `M7_0_0` is the boot coordinator and the sole executor of system-global initialization. `CM7_2` in image `M7_0_2` must remain held or wait at a bounded software barrier until `CM7_0` declares global initialization complete. After that release, both images enter their independent application initialization and start their own FreeRTOS schedulers.
- Global initialization ownership: only `CM7_0` configures the system clock tree/PLL, global mode and clock distribution, shared pin/routing state, shared-resource ownership, and the eventual standalone release/start address of `CM7_2`. A global hardware configuration API must not be called independently by both images merely because both projects contain a generated driver copy.
- Core-local initialization: each executing application core initializes its own vector-table/NVIC state, FreeRTOS kernel objects, tasks, diagnostics, and exclusively owned peripheral instances. Consequently `IntCtrl_Ip_Init()` remains a per-core call with a per-core configuration, while `Clock_Ip_Init()` is a `CM7_0`-only system call.
- Peripheral rule: each physical peripheral instance has exactly one initialization owner. The proven primary timebase is `STM_0` owned by `CM7_0`; the exact dedicated STM instance for `CM7_2` remains `Proposed` until S32 Configuration Tools validates its ownership, interrupt route, and clock.
- Debug boundary: a debugger group launch may currently start or prepare both cores, but it is not the production boot contract. If the debugger releases `CM7_2` before `CM7_0` is ready, the secondary image must still wait at the same software readiness barrier used by hardware and QEMU.
- Interface direction: separate the current board initialization into a primary-only system layer (for example `Bsp_SystemInit()`) and a per-image local layer (for example `Bsp_CoreInit()`). The exact readiness transport, timeout, shared-memory/cache attributes, and failure reaction remain open and must be decided before `TaskB` uses clocked peripherals.
- Reason: this matches automotive master/worker startup practice, prevents concurrent or repeated programming of global registers, preserves independent AMP application execution, and gives hardware and QEMU one deterministic startup sequence to compare.

## Open decisions

- Full package/build identifiers and applicable safety documentation to freeze beyond S32DS 3.6.5, RTD 7.0.0, GCC 10.2, and FreeRTOS V11.1.0.
- Debug probe/firmware and whether onboard OpenSDA or an external PEmicro probe is the primary path.
- Git repository boundary: entire workspace versus only source project plus documentation (recommended: exclude `.metadata` and generated build output).
- Coding standard target (project convention only, MISRA-oriented, or formal MISRA compliance).
- Host-runner language and artifact format.
- Required QEMU machine/core configuration and currently implemented peripherals.
- CM7_0-to-CM7_2 `SYSTEM_READY` transport, timeout, cache-safe storage, and failure reaction.
- Available lab equipment: oscilloscope/logic analyzer, CAN interface, LIN interface, Ethernet peer, loopback wiring, and SD card.
