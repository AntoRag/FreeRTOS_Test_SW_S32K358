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

## Phase 1 - Bare-metal observability smoke test

Objective: prove clocks, pin mux, GPIO, timing, and a host-visible output without FreeRTOS complexity.

Work:

- deterministic LED color sequence using D16 or D22;
- serial banner through LPUART6 containing protocol version, build identity, target, core, reset cause, and clock values;
- bounded polling only; no hidden infinite waits;
- characterize SW2/SW3 polarity and record the manual discrepancy resolution.

Hardware gate H1:

- measured LED sequence and timing match expectation across three cold boots;
- serial banner is captured without corruption;
- each button state is observed pressed/released and polarity is recorded;
- debugger breakpoints and peripheral/register views agree with visible behavior.

QEMU gate Q1: boot, UART banner, and GPIO-visible state sequence match the hardware trace subject to documented timing tolerance.

## Phase 2 - FreeRTOS kernel baseline

Objective: prove scheduler, tick, task states, synchronization, memory policy, and failure hooks.

Work:

- use the exact installed kernel version and its matching port/configuration;
- create a small fixed task set: supervisor, heartbeat, and test worker;
- prefer static task/queue/timer allocation for determinism;
- configure `configASSERT`, stack-overflow hook, malloc-failed hook (if dynamic allocation remains enabled), and runtime/stack diagnostics;
- test delay-until periodicity, queue or direct notification, mutex/priority behavior, and ISR-safe API rules.

Hardware gate H2:

- scheduler runs for a defined soak interval with no assert/reset;
- task periods and tick count are within recorded tolerances;
- task communication passes known sequences;
- stack high-water marks and heap/static-memory policy are recorded;
- an intentional negative build or controlled fault proves each enabled diagnostic hook.

QEMU gate Q2: task/event sequence matches; wall-clock timing differences are represented as explicit tolerances.

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

Objective: establish the timing reference needed by all later tests.

Tests: PIT/STM periodic and one-shot interrupts, wraparound, priority/nesting, ISR-to-task notification, timeout accuracy, and optional latency measurement with GPIO toggles.

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

## Phase 11 - Multicore and inter-core behavior

Objective: introduce independent `CM7_2` deliberately.

Work: document boot ownership, images/linker regions, shared-memory contract, cache maintenance, SEMA42/MU/IPCF choice, interrupt routing, and failure containment. Decide whether FreeRTOS runs on one or both application cores.

Hardware gate H11: independent core heartbeat, deterministic message exchange, shared-memory stress, reset/restart behavior, and debugger attach are proven without corrupting the primary core.

## Phase 12 - Watchdog, reset, power, and selected safety mechanisms

Objective: validate lifecycle and fault behavior without risking board recovery.

Tests: SWT service/expiry, reset-cause capture, controlled clock/error injection where supported, FCCU observation, standby/wakeup, and recovery procedure. FS26 normal mode is a separate hardware decision because the board defaults to FS26 debug mode and requires component changes.

Hardware gate H12: every induced fault has a predicted observable reaction and a demonstrated recovery path.

## Phase 13 - Regression and model qualification

Objective: make hardware-vs-QEMU comparison routine.

Work: automated host runner, target capability discovery, allowed-difference manifest, golden semantic traces, CI-friendly exit status, coverage matrix, and release report.

Exit: every claimed QEMU/ASIC feature maps to a stable test ID, a real-board `H_PASS`, a model `Q_PASS`, and an reviewed parity result.
