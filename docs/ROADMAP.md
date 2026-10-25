# Phased Roadmap

The phases are sequential baselines, not calendar estimates. Each phase adds one class of uncertainty and ends with a physical-board gate. QEMU replay begins as soon as the model can boot the corresponding image.

## Phase 0 - Reproducible baseline

Objective: freeze the software/tool identity and create a clean, reproducible S32DS project baseline. Basic board health and generic debugger operation are user-confirmed prerequisites.

Work:

- record the already-working board revision, relevant jumper positions, and probe identity without repeating generic bring-up tests;
- confirm S32DS, installed S32K3 RTD/PlatformSDK, FreeRTOS, GCC, S32 Configuration Tools, and debugger versions;
- decide whether to start from an NXP board example or a blank S32K358 project (a matching example is preferred when available);
- start with the lockstep application core (`CM7_0`/checker) only; park `CM7_2` unless project generation requires a minimal secondary image;
- establish version control and archive the untouched generated baseline;
- generate, build, flash, reset, halt, step, and inspect registers.

Hardware gate H0:

- `Debug_FLASH` builds with zero compiler/linker errors and produces a recorded ELF/map;
- the project image flashes through the known-working debug path, reaches `main()`, and survives reset/reflash;
- evidence is entered in `TEST_LOG.md`.

## Combined Phase 1/2 - Dual-core AMP FreeRTOS and observability

Objective: establish two independent FreeRTOS instances, periodic task execution on both application cores, and enough board-visible observability to make the RTOS task structure the foundation for all later peripheral tests.

Architecture:

- preserve the existing two projects, two `.mex` files, two linker maps, and two ELF images;
- run a single-core FreeRTOS V11.1.0 instance in each image (`configNUMBER_OF_CORES = 1`; the installed generated configuration does not emit a `configUSE_SMP` macro);
- `M7_0_0` owns global clock/startup policy and the initial UART control channel;
- `M7_0_2` owns only explicitly assigned resources and never repeats global initialization;
- keep one owner per peripheral; introduce shared-memory/MU synchronization before any cross-core shared service.

Staged work and evidence:

1. **H1A - primary kernel (`FUNCTIONAL PASS`):** attach the NXP FreeRTOS and Cortex-M7 port components to `M7_0_0`, select FreeRTOS in OSIF, configure one STM compare interrupt, generate, clean-build, flash, start the scheduler, and prove that the interrupt releases into one blocked task. Proven by `RUN-20261024-005` and the complete TaskA regressions. Immediate ISR-to-task preemption latency remains unmeasured.
2. **H1B - secondary kernel (`FUNCTIONAL PASS` in the integrated run):** repeat the same integration independently for `M7_0_2` and prove its core-owned STM release path and scheduler. `RUN-20261025-015` proves the STM2/TaskB path and scheduler while both images execute; no earlier secondary-only qualification artifact was retained, and immediate preemption latency remains unmeasured.
3. **H1C - simultaneous AMP (`FUNCTIONAL PASS`):** group-launch both images and prove both independent task counters progress without observed asserts or resets. `RUN-20261025-015` proves this runtime behavior with both fatal states zero and exact interrupt-to-task count equality; exact ELF/map artifacts were not archived for a formal memory-overlap check.
4. **H1D - independent timing-task model (`IMPLEMENTED`, observability incomplete):** the A-family and B-family 10/100/1000 ms task sets and their dedicated STM0/STM2 releases are hardware-proven. CM7_2 Idle-hook parity, separate LED/trace indications, and the initial LPUART6 reporting service are not implemented or proven yet.

Current combined-gate status (2026-10-25): `IN PROGRESS`. The dual-core scheduler/task runtime sub-gate passed and is checkpointed by commit `49b4ad7`. The complete H1/2 exit criteria below have not all been satisfied.

Engineering work:

- derive configuration and interrupt-priority settings from the installed NXP S32K358 single-core example, not from generic upstream defaults;
- prefer static task and synchronization-object allocation, with an explicitly documented exception if dynamic allocation is temporarily needed for bring-up;
- enable `configASSERT`, stack-overflow detection, failure hooks, and task stack high-water inspection;
- give each periodic task its own ISR notification and timing diagnostics; do not use FreeRTOS delay APIs as the cyclic activation source;
- enable preemption and verify the ISR-to-task context switch path using a FreeRTOS-safe STM interrupt priority;
- record activation and execution counts while deferring the overrun reaction policy;
- use equal priority for tasks in the same rate class and rate-monotonic priority ordering between the 10 ms, 100 ms, and 1000 ms classes;
- characterize SW2/SW3 polarity and record the board-manual discrepancy;
- enforce the application-owned Power/MC_ME release contract so `M7_0_2` cannot execute before `M7_0_0` completes global initialization, and keep any later primary acknowledgement wait bounded;
- record per-core clock, tick, cache/MPU, stack, and linker-memory settings.

Combined hardware gate H1/2:

- both independent schedulers survive repeated cold boot and a defined soak interval;
- periodic task counts and LED/trace periods are within recorded tolerances on both cores;
- the debugger identifies the correct task set in each ELF/session;
- no assertion, stack overflow, unexpected reset, memory overlap, or unowned peripheral access occurs;
- the external UART banner identifies build, physical core, RTOS version, reset cause, and clock values;
- enabled diagnostic hooks are exercised through a safe controlled negative test.

QEMU gate Q1/2: run the same two AMP images and compare per-core task/event sequences. If the model initially supports only `M7_0_0`, record the missing secondary-core capability explicitly; a single-core run does not qualify as full AMP parity.

## Phase 3 - Versioned test harness

Objective: turn examples into a reusable validation product.

Work:

- implement a test registry with stable IDs and metadata;
- implement `INFO`, `LIST`, `RUN <id>`, `RUN_GROUP <name>`, and `ABORT` commands;
- emit the versioned ASCII result protocol defined in `ENGINEERING_RULES.md`;
- add timeouts, setup/execute/teardown isolation, reset-cause reporting, and a monotonic event sequence number;
- separate board support, driver adaptation, test cases, and transport.

Hardware gate H3:

- a host capture can enumerate, run, abort, and repeat tests;
- unknown/malformed commands fail safely;
- results are machine-parseable and identical across three runs except declared volatile fields.

QEMU gate Q3: the same host-side parser and command set work unmodified.

## Phase 4 - GPIO, external interrupt, and pin-mux model

Objective: characterize SIUL2 GPIO and interrupt behavior.

Tests: LED read/write patterns, button polling, edge interrupt, interrupt masking, debounce policy, pin reset state, and read-back behavior.

Hardware gate H4: logic-visible GPIO levels and interrupt counts match expected sequences; no spurious event survives debounce rules.

## Phase 5 - Timers, interrupts, and timebase

Objective: extend and stress the STM activation baseline and characterize additional timer/interrupt behavior needed by later tests.

Tests: PIT/STM periodic and one-shot interrupts, wraparound, priority/nesting, ISR-to-task notification latency, timeout accuracy, and optional latency measurement with GPIO toggles.

Hardware gate H5: scope/logic-analyzer or debugger timestamp evidence meets defined frequency, jitter, and count tolerances.

## Phase 6 - LPUART transport and data-path stress

Objective: validate the primary control/observation channel under interrupt-driven load.

Tests: baud configurations, frame formats needed by the project, RX/TX interrupts, ring-buffer boundaries, overrun/error handling, task wakeups, and sustained traffic.

Hardware gate H6: host-to-board and board-to-host payloads pass CRC/sequence checks with recorded rate and duration.

## Phase 7 - ADC and I2C devices

Objective: cover analog conversion and a simple register-oriented serial bus.

Tests:

- ADC2_S19 potentiometer monotonicity, limits, repeatability, calibration, and interrupt/DMA path later;
- LPI2C discovery and register reads for the P3T1750 and FXLS8961, including NACK/timeout recovery.

Hardware gate H7: potentiometer sweep and sensor identity/data are captured with raw values and tolerances. Separate MCU-peripheral behavior from external-device behavior in QEMU results.

## Phase 8 - LPSPI and eDMA

Objective: validate synchronous transfers and bus-master behavior.

Tests: external loopback fixture, mode/word-size/clock cases, FIFO limits, interrupt path, eDMA transfer, scatter/gather only after the basic path, and cache coherency where applicable.

Hardware gate H8: transmitted/received patterns, DMA status, and memory guards pass across boundary sizes. Do not use the FS26 control bus as the first experimental SPI target.

## Phase 9 - Automotive communication

Objective: characterize FlexCAN first, then LIN, using known-good external peers.

Tests: CAN classic and FD nominal/data rates, filters, loopback, error states, recovery, timestamping; LIN controller/responder cases after test equipment is available.

Hardware gate H9: an external analyzer/peer log and the board result stream agree on IDs, payloads, timing, and induced errors.

## Phase 10 - Ethernet and storage/external memory

Objective: cover higher-complexity bus masters after interrupts and DMA are trusted.

Tests: RMII link/frames and descriptors; QuadSPI HyperRAM patterns/boundaries/cache behavior; uSDHC identification and block I/O with a sacrificial card.

Hardware gate H10: external packet/storage evidence and memory integrity checks pass. Destructive storage tests require explicit media scope.

## Phase 11 - Advanced inter-core communication and fault containment

Objective: stress the AMP architecture after both independent FreeRTOS instances are already established.

Work: finalize the shared-memory contract, cache maintenance, SEMA42/MU/IPCF choice, interrupt routing, back-pressure, malformed-message handling, timeout/recovery, and failure containment.

Hardware gate H11: independent core heartbeat, deterministic message exchange, shared-memory stress, reset/restart behavior, and debugger attach are proven without corrupting the primary core.

## Phase 12 - Watchdog, reset, power, and selected safety mechanisms

Objective: validate lifecycle and fault behavior without risking board recovery.

Tests: SWT service/expiry, reset-cause capture, controlled clock/error injection where supported, FCCU observation, standby/wakeup, and recovery procedure. FS26 normal mode is a separate hardware decision because the board defaults to FS26 debug mode and requires component changes.

Hardware gate H12: every induced fault has a predicted observable reaction and a demonstrated recovery path.

## Phase 13 - Regression and model qualification

Objective: make hardware-vs-QEMU comparison routine.

Work: automated host runner, target capability discovery, allowed-difference manifest, golden semantic traces, CI-friendly exit status, coverage matrix, and release report.

Exit: every claimed QEMU/ASIC feature maps to a stable test ID, a real-board `H_PASS`, a model `Q_PASS`, and an reviewed parity result.
