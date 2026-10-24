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

## D-018 - CM7_2 hardware release and shared startup mailbox

- Status: ACCEPTED
- Date: 2026-10-24
- Decision: Separate physical core release from logical readiness. In the standalone boot path, `CM7_0` programs the `CM7_2` start address and releases the secondary core through `MC_ME`. The persistent `SYSTEM_READY` condition is communicated through a small, versioned boot mailbox in shared SRAM. A cross-core interrupt is not the initial source of truth and is not required for the first implementation.
- Initial protocol: `CM7_0` explicitly initializes the mailbox, reports initialization progress/failure, completes system-global initialization, publishes `SYSTEM_READY`, and releases or permits `CM7_2` to proceed. `CM7_2` waits with a bounded timeout, validates the mailbox, performs only its core-local and owned-peripheral initialization, then publishes `CORE2_READY`. Debugger-started and QEMU-started executions must obey the same logical readiness check even if those environments release the core themselves.
- Memory contract: the mailbox must occupy one explicitly reserved address range that is identical in both ELF linker maps, is visible to both cores, and is configured shareable and non-cacheable. It must not be an ordinary per-image global and must be excluded from startup copy/zero ranges that either core could overwrite. The structure has a magic value, protocol version, explicit states, and separate single-writer fields for `CM7_0` and `CM7_2`; additional integrity fields may be introduced by the safety gate.
- Ordering contract: publishing or consuming a mailbox state transition uses Arm data-memory barriers. The producer completes payload writes, executes `__DMB()`, then publishes the state. After observing the state, the consumer executes `__DMB()` before consuming the associated payload. `volatile` remains necessary for polled fields but is not a substitute for memory ordering, and `__DMB()` is not a substitute for cache maintenance if the memory attributes are later changed.
- Notification policy: the first implementation uses bounded polling so it introduces no additional interrupt-controller, MSCM MSI, or MU dependency. This active polling is an intentionally temporary bootstrap mechanism, not the preferred steady-state inter-core notification service. Phase 11 will evaluate an `MSCM` directed interrupt or `MU` event to wake the receiving core; after wake-up, the receiver must still validate the persistent mailbox state because an interrupt alone is never the readiness state.
- Failure policy: timeout, invalid magic/version/state, or a peer-reported failure must enter a deterministic diagnostic failure path rather than starting the local scheduler or using shared hardware. Exact timeout, retry/reset reaction, mailbox address/size, and warm-reset/stale-state handling remain `Open` until implemented and tested.
- Evidence boundary: fixed mailbox placement, the successful-path producer/consumer barriers, and cross-core visibility are hardware-confirmed by `RUN-20261024-011` and `RUN-20261024-012`. `MC_ME` release, timeout/failure behavior, warm-reset handling, hardware-assisted wake-up, and `CORE2_READY` acknowledgement remain unconfirmed.
- Reason: state plus optional notification is robust against lost or early interrupts, supports debugger and QEMU parity, and gives the two independently linked AMP images an explicit startup contract.

## D-019 - Standard runtime entry and explicit per-core boot/application entrypoints

- Status: ACCEPTED
- Date: 2026-10-24
- Decision: Preserve the toolchain and NXP runtime entry sequence in both images: the linker entry remains `Reset_Handler`, the startup assembly performs C-runtime initialization, and then calls the standard C `main()`. Each `main()` is a minimal adapter into project-owned functions rather than being renamed or replaced in startup/linker configuration.
- Primary image: `M7_0_0` follows `Reset_Handler -> main() -> Core0_Boot() -> Core0_Main()`. `Core0_Boot()` owns boot coordination and system-global initialization; `Core0_Main()` owns CM7_0-local initialization, CM7_0-owned services, TaskA creation, and scheduler startup.
- Secondary image: `M7_0_2` follows `Reset_Handler -> main() -> Core2_Boot() -> Core2_Main()`. `Core2_Boot()` validates and waits on the bounded boot-mailbox protocol; `Core2_Main()` owns CM7_2-local initialization, CM7_2-owned services, TaskB creation, and scheduler startup.
- Naming boundary: these functions are logical software entrypoints inside the existing two application ELFs. They do not create a separately linked or independently updateable bootloader. A future bootloader/application split would require separate images, flash partitions, validation, and controlled handover and is outside this decision.
- Migration rule: first perform a behavior-preserving CM7_0 source refactor and repeat the existing TaskA hardware regression. Add mailbox state, `MC_ME` release, and the CM7_2 application path only in later experiments so each hardware test introduces one architectural variable.
- Reason: the split makes boot ownership explicit and testable while retaining the generated startup, debugger expectations, C-runtime initialization, and QEMU-compatible ELF entry semantics.

## D-020 - Boot mailbox V1 address, layout, and ownership

- Status: ACCEPTED
- Date: 2026-10-24
- Decision: Boot-mailbox protocol V1 occupies the fixed 256-byte range `0x204BFF00` through `0x204BFFFF`. All CM7_0/CM7_2 FLASH and RAM linker scripts reserve this range as the `NOLOAD` output section `.boot_mailbox`, aligned to 32 bytes, with identical start/end symbols and a linker size assertion.
- Memory attributes: the generated startup MPU region beginning at `0x20480000` retains its 256 KiB size and therefore covers the mailbox as normal, shareable, non-cacheable, read/write, execute-never memory on both application cores. The smaller linker allocation named `int_sram_shareable` prevents ordinary shared data from occupying the mailbox but does not reduce the MPU region.
- Initialization rule: `CM7_0` owns ECC initialization of the complete shared-SRAM range, including the mailbox. The primary startup loop must use 64-bit stores through `0x204BFFFF` before entering C; the linker therefore supplies `__INT_SRAM_SHARE_END = 0x204C0000`. `BootSync_PrimaryInitialize()` still explicitly initializes every protocol word for deterministic state and warm-reset handling, but it is not responsible for establishing ECC. `CM7_2` must not repeat shared-SRAM ECC initialization. The mailbox is never assumed to contain zero after reset merely because its linker section is `NOLOAD`.
- V1 fields: fixed-width 32-bit words contain magic, protocol version, system state, system error, CM7_2 state, and CM7_2 error; the remainder is reserved and initialized to zero. The accepted initial system states are `INVALID`, `INITIALIZING`, `READY`, and `FAILED`; the accepted CM7_2 states are `RESET`, `WAITING`, `READY`, and `FAILED`.
- Ownership: after primary initialization, only `CM7_0` writes the system state/error fields and only `CM7_2` writes the CM7_2 state/error fields. `CM7_0` is allowed to initialize the complete mailbox at the start of a new boot sequence before publishing `INITIALIZING`.
- Ordering: a producer completes payload writes, executes a `dmb sy` barrier through the boot-sync abstraction, and then publishes its state word. A consumer that observes the awaited state executes the same barrier before consuming payload. A state transition is valid only when magic and protocol version also match.
- Bring-up sequence: CM7_0 initialization and `SYSTEM_READY` publication were validated first, followed by successful bounded CM7_2 consumption. Timeout behavior, warm-reset generation handling, `CORE2_READY` publication, and `MC_ME` physical release remain separate later experiments.
- Evidence boundary: both maps place the 256-byte V1 object at `0x204BFF00`; CM7_0 runtime publication and CM7_2 cross-core consumption are hardware-confirmed by `RUN-20261024-011` and `RUN-20261024-012`. `RUN-20261025-015` additionally confirms that firmware-owned 64-bit startup ECC initialization through `0x204C0000` supports simultaneous TaskA/TaskB execution with the stock PEmicro macro disabled. Timeout/failure behavior, warm-reset behavior, and notification-driven wake-up remain unconfirmed.

## D-021 - Multicore debug launch policy and STM debug freeze

- Status: ACCEPTED
- Date: 2026-10-24
- Decision: Use the generated multicore Launch Group for normal interactive debugging of the AMP system. A primary-only debug launch that leaves `CM7_2` halted is diagnostic-only and is not an accepted execution or qualification path.
- STM policy: retain `StmFreezeEnable = true` for normal multicore debugging so the STM timebase stops while the multicore target is halted. The temporary `StmFreezeEnable = false` configuration was used only to isolate the debugger interaction and is not the selected baseline.
- Evidence: with `STM_0.CR = 0x2F03` (`TEN=1`, `FRZ=1`), a primary-only launch left `STM_0.CNT` and all STM-driven TaskA counters stopped while the FreeRTOS Idle counter advanced. With only `FRZ` cleared, `STM_0.CR` became `0x2F01`, `STM_0.CNT` advanced from `17660025` to `36236283`, and the 10/100/1000 ms interrupt and task-execution counts matched at `256/26/3`. The relevant clock and MC_ME observations were unchanged, isolating STM debug freeze as the cause of the apparent task stoppage.
- Boundary: this result characterizes debugger behavior; it does not indicate a standalone firmware clock or scheduler failure. Cold-boot behavior must be evaluated without treating a debugger-created core state as the production boot contract.

## D-022 - CM7_2 MC_ME boot-address derivation and Power ownership

- Status: PROPOSED
- Date: 2026-10-24
- Proposed value: configure `McuCore4Configuration.McuCoreBootAddress = 0x00700000` for the `Debug_FLASH` CM7_2 image. `CORE4` is the MC_ME core identifier used for the independent `CM7_2` application core.
- Derivation: `FreeRTOS_Test_SW_M7_0_2/Project_Settings/Linker_Files/linker_flash_c2_s32k358.ld` defines `int_flash_c2` with `ORIGIN = 0x00700000`, places the secondary initialization/vector-table section in that region, and exports `__ENTRY_VTABLE = __INIT_INTERRUPT_START`. `MC_ME.PRTN0_CORE4_ADDR` must point to the secondary reset vector table, so the candidate address comes from the secondary image's own linker layout rather than from an unrelated example.
- Example boundary: the NXP `S32K358_Multicore_Start_CM7_2_400` example uses `0x00800000` because its secondary image has a different flash layout. That literal must not be copied into this project.
- Power ownership: only `M7_0_0` contains and calls the Power IP driver. The proposed mode configuration places Partition 0 and CM7_2 under Power/MC_ME control, leaves CM7_0 and its lockstep checker outside core control, and leaves COFB peripheral-clock programming disabled so `Clock_Ip_Init()` remains the owner of the established peripheral-clock configuration.
- Confirmation gate: before promotion to `ACCEPTED`, the regenerated CM7_2 map/ELF must show the final `__ENTRY_VTABLE` value, generated Power data must contain the same boot address, and a hardware cold-boot test must prove `CM7_0 -> Power_Ip_SetMode() -> CM7_2 Reset_Handler/Core2_Boot()` without debugger-assisted secondary release.
- Current evidence: `RUN-20261025-015` proves the application-owned Power/MC_ME release path during a multicore debug-group session after disabling the conflicting PEmicro device macro. This does not yet satisfy the no-debugger cold-boot confirmation gate.

## D-023 - PEmicro startup macro must not own application MC_ME state

- Status: ACCEPTED
- Date: 2026-10-25
- Decision: The stock PEmicro S32K358 startup macro must not execute unmodified when the application owns CM7_2 release through Power/MC_ME. The inspected S32DS 3.6.5 plug-in macro `supportFiles_ARM/NXP/S32K3xx/S32K358.mac` enables MC_ME partition clocks, writes temporary SRAM vectors, rewrites the CM7_0 and CM7_2 MC_ME boot-address/control registers, and resets both cores. Those operations conflict with the application boot contract and the linked CM7_2 flash entry address.
- Confirmed diagnostic workaround: the operator renamed the installed macro to `S32K358_.mac`, preventing PEmicro from finding and executing it. With the stock macro absent, the application-owned MC_ME sequence released CM7_2 correctly. This follows the analogous workaround in the NXP S32K324 dual-core PEmicro guide. The inspected renamed S32K358 macro has SHA-256 `8D4092F0348C237A7E5306F972BC9F5F205B3E7BD6DA3F2DBEC0A49C8E642732`.
- Reproducibility boundary: renaming a file inside the S32DS installation is a host-local workaround, not a repository-contained production solution. S32DS/PEmicro upgrades can restore the file, and another workstation will not inherit the change. The debug procedure must therefore check and record the macro state and exact plug-in version.
- RAM/ECC consequence: the same stock macro also performs a DMA-based SRAM initialization. Disabling the complete macro exposed an ECC fault when C code first wrote the mailbox excluded from the primary startup ECC range. Firmware startup, not the debugger, must initialize the complete SRAM/ECC range needed by the application. The accepted correction is to extend the existing primary `startup_cm7.s` shared-SRAM 64-bit initialization loop through the mailbox end at `0x204C0000`; do not reproduce the macro's MC_ME/core-reset side effects.
- Source: NXP Community, `Dual cores debugging with PEMicro's MULTILINK by S32DS for S32K324`, published 2022-06-22. The guide is precedent for disabling a device macro when it leaves the secondary start address at zero; S32K358 applicability was established by inspecting the installed S32K358 macro and by this board experiment.

## Open decisions

- Full package/build identifiers and applicable safety documentation to freeze beyond S32DS 3.6.5, RTD 7.0.0, GCC 10.2, and FreeRTOS V11.1.0.
- Debug probe/firmware and whether onboard OpenSDA or an external PEmicro probe is the primary path.
- Git repository boundary: entire workspace versus only source project plus documentation (recommended: exclude `.metadata` and generated build output).
- Coding standard target (project convention only, MISRA-oriented, or formal MISRA compliance).
- Host-runner language and artifact format.
- Required QEMU machine/core configuration and currently implemented peripherals.
- Exact boot-mailbox polling timeout value, warm-reset generation/stale-state scheme, and deterministic timeout/failure reaction.
- Available lab equipment: oscilloscope/logic analyzer, CAN interface, LIN interface, Ethernet peer, loopback wiring, and SD card.
