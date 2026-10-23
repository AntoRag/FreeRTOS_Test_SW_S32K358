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

- Status: ACCEPTED
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

- Status: PROPOSED
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

- Status: PROPOSED
- Date: 2026-10-23
- Decision: Treat the two per-core `.mex` files as generated views of one MCU configuration. Keep one reviewed system-level source of truth for clocks, pins, peripheral ownership, and initialization order; assign every peripheral and global initialization action to exactly one core.
- Initial ownership proposal: `CM7_0` owns boot, clocks, system/global initialization, and the first peripheral tests. `CM7_2` remains parked/minimal until a test phase explicitly assigns work to it.
- Baseline gate: use one frozen S32DS/S32CT/RTD version, validate and regenerate both `.mex` files with that tool set, review the diff, clean-build both images, and verify the group image on the real board.
- Compliance boundary: S32 Configuration Tools validation and a successful build establish configuration/tool consistency only. ISO 26262/ASIL claims additionally require the applicable NXP safety manual and assumptions of use, safety requirements and traceability, independent review, and recorded verification evidence.

## Open decisions

- Exact S32DS/RTD/FreeRTOS versions to freeze.
- Whether FreeMASTER is useful enough to include in the baseline; it is not required for the first UART protocol.
- Debug probe/firmware and whether onboard OpenSDA or an external PEmicro probe is the primary path.
- Git repository boundary: entire workspace versus only source project plus documentation (recommended: exclude `.metadata` and generated build output).
- Coding standard target (project convention only, MISRA-oriented, or formal MISRA compliance).
- Host-runner language and artifact format.
- Required QEMU machine/core configuration and currently implemented peripherals.
- Available lab equipment: oscilloscope/logic analyzer, CAN interface, LIN interface, Ethernet peer, loopback wiring, and SD card.
