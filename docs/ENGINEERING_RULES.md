# Engineering Rules

These rules are proposed defaults until accepted in `DECISIONS.md`.

## Architecture

Keep four layers with one-way dependencies:

1. `app` - startup policy, supervisor, and test selection.
2. `test` - target-independent test intent, registry, assertions, and results.
3. `bsp` - named board resources such as user LEDs, buttons, and serial console.
4. `platform` - NXP RTD/S32 Configuration Tools adapters and generated configuration.

Test cases must not reach directly into generated configuration symbols when a board/platform adapter can express the intent. Hardware and QEMU builds should share the same test layer.

## C naming convention

Recommended baseline: C99 application code. C++ is deferred unless a concrete need is accepted.

| Element | Convention | Example |
|---|---|---|
| Source/header | lower snake case | `test_runner.c`, `board_console.h` |
| Public function | Module prefix + Pascal action | `TestRunner_Run()` |
| Private function | lower camel case, `static` | `parseCommand()` |
| Type | Pascal case + `Type` | `TestResultType` |
| Enum value | upper snake case | `TEST_RESULT_PASS` |
| Macro/config | module prefix + upper snake case | `TEST_RUNNER_MAX_CASES` |
| Local/parameter | lower camel case | `testId`, `elapsedTicks` |
| File-static object | `s_` + lower camel case | `s_testRegistry` |
| Extern global | avoid; if required, `g_` + lower camel case | `g_testSummary` |
| Boolean | positive predicate | `isInitialized`, `hasTimedOut` |
| Units | suffix the name | `timeoutMs`, `periodTicks`, `frequencyHz` |

Do not use leading underscores, unexplained abbreviations, type-encoded Hungarian notation, or names that differ only by case.

## Test identity and result contract

- Stable ID: `<DOMAIN>-<three digits>`, for example `GPIO-001`, `RTOS-004`, `CAN-012`.
- A test ID never changes meaning. Replace obsolete tests with a new ID and mark the old one deprecated.
- Each case declares: purpose, prerequisites, timeout, destructive flag, required capabilities, setup, execute, teardown, expected result, and tolerances.
- Outcomes: `PASS`, `FAIL`, `SKIP`, `ERROR`, `TIMEOUT`.
- `SKIP` requires a machine-readable reason and never counts as pass.

Version 1 line protocol (proposed):

```text
TST|1|INFO|build=<id>|target=S32K358|core=CM7_0|reset=<cause>
TST|1|START|seq=<n>|id=GPIO-001
TST|1|ASSERT|seq=<n>|id=GPIO-001|name=<name>|actual=<value>|expected=<value>
TST|1|RESULT|seq=<n>|id=GPIO-001|status=PASS|duration_us=<n>
```

Rules: ASCII, one record per CRLF-terminated line, fixed field order per record type, no locale-dependent numbers, escaped reserved characters, and no free-form text in mandatory fields. Diagnostic text may follow as an optional escaped field.

## Determinism

- Every wait has a timeout.
- Every test restores the peripheral to a documented state or requests a controlled reset.
- Seed pseudo-random patterns explicitly and report the seed.
- Separate semantic tolerances from host wall-clock timing.
- Record caches, clock frequencies, optimization level, and execution location (FLASH/RAM).
- Check guards around DMA buffers and apply explicit cache maintenance.
- Do not use logging from timing-critical ISR paths; record compact events and drain them later.

## FreeRTOS policy

- Use AMP: each ELF contains one independent single-core kernel with `configNUMBER_OF_CORES == 1`; do not enable FreeRTOS SMP.
- Only the `M7_0_0` image performs global clock/system initialization. The `M7_0_2` image waits for the defined startup contract before using shared or globally initialized services.
- Every peripheral, interrupt route, DMA channel, and associated buffer has one recorded owning core at a time.
- Use only `...FromISR` APIs from interrupt context and honor the port's maximum syscall interrupt priority.
- Prefer direct-to-task notifications for one-to-one events; queues for copied messages; mutexes only for ownership with priority inheritance.
- Implement every periodic activity as an independent task that blocks on its own task notification. Do not use `vTaskDelay()` or `vTaskDelayUntil()` as the application cyclic-release mechanism.
- Use one core-owned STM instance with continuous compare channels for the 10 ms, 100 ms, and 1000 ms releases. Each compare ISR releases the A and B tasks in its rate class with ISR-safe task notifications.
- Enable preemption (`configUSE_PREEMPTION == 1`) and request a context switch from the ISR when a higher-priority task becomes ready.
- Use 10 ms, 100 ms, and 1000 ms rate classes. Give tasks in the same class equal priority and never depend on their relative execution order. Order classes rate-monotonically: 10 ms above 100 ms above 1000 ms.
- Assign STM interrupt priorities within the FreeRTOS port's permitted syscall-interrupt range before calling any `...FromISR` API.
- Instrument pending releases and execution counts, but do not impose an overrun handling policy until one is explicitly accepted.
- Use the kernel Idle task/Idle hook as background activity. The Idle hook must remain non-blocking, bounded, and free of peripheral waits or functional tests that may stall.
- Prefer static allocation for kernel objects in the test harness.
- Enable and prove assertions, stack-overflow detection, and useful high-water/runtime diagnostics.
- Keep interrupt priority assignments in one reviewed table.

## Generated code and S32DS

- The `.mex` configuration is a source artifact; generated output is derived evidence.
- Make configuration changes in S32 Configuration Tools, validate, generate, inspect the diff, then build.
- Never treat red editor/indexer markers as compiler errors without a build log.
- Keep FLASH and RAM configurations distinct in evidence. Hardware release qualification uses FLASH unless a test explicitly targets RAM execution.
- Archive build console output, ELF/map size, debugger configuration, and test transcript for each milestone.

## Definition of done for one test

- requirement and stable test ID exist;
- expected behavior comes from an identified source or an explicit project decision;
- build is warning-reviewed;
- test passes repeatedly on the board with evidence;
- negative/error path is exercised where safe;
- QEMU result and allowed differences are recorded when that model exists;
- documentation and decision/test logs are updated.
