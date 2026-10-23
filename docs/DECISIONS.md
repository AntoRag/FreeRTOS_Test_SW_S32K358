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

- Status: PROPOSED
- Date: 2026-10-23
- Decision: separate `app`, target-independent `test`, `bsp`, and NXP `platform` layers.
- Reason: preserves test intent across physical hardware, QEMU, and later ASIC models.

## D-008 - Initial project source

- Status: SUPERSEDED by D-012
- Date: 2026-10-23
- Decision: prefer a current NXP example matching S32K358/FRDM-A-S32K358, RTD, FreeRTOS, and available debugger; otherwise create a minimal blank project with those exact live options.
- Reason: a board-tested example reduces startup and linker uncertainty while the user learns S32DS.
- Confirmation needed: installed example catalog and exact package versions.

## D-009 - FreeMASTER role

- Status: PROPOSED
- Date: 2026-10-23
- Decision: Do not treat FreeMASTER as part of FreeRTOS or as a prerequisite for the kernel. Retain it only if we deliberately use it for runtime variable observation, signal recording, parameter tuning, or application commands.
- Current state: the SDK is attached to both core projects, but the firmware does not currently initialize or poll the FreeMASTER driver.
- Recommendation: keep the first UART test protocol independent of FreeMASTER; evaluate FreeMASTER later as an additional engineering observability channel.

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

## Open decisions

- Full package/build identifiers and applicable safety documentation to freeze beyond S32DS 3.6.5, RTD 7.0.0, GCC 10.2, and FreeRTOS V11.1.0.
- Whether FreeMASTER is useful enough to include in the baseline; it is not required for the first UART protocol.
- Debug probe/firmware and whether onboard OpenSDA or an external PEmicro probe is the primary path.
- Git repository boundary: entire workspace versus only source project plus documentation (recommended: exclude `.metadata` and generated build output).
- Coding standard target (project convention only, MISRA-oriented, or formal MISRA compliance).
- Host-runner language and artifact format.
- Required QEMU machine/core configuration and currently implemented peripherals.
- Available lab equipment: oscilloscope/logic analyzer, CAN interface, LIN interface, Ethernet peer, loopback wiring, and SD card.
