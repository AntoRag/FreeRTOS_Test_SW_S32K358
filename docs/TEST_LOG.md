# Hardware and Model Test Log

Hardware gate H0 passed on 2026-10-23. Later phase gates remain unexecuted.

User-provided prerequisite: the FRDM-A-S32K358 board is known working and has already been tested. This is accepted as board-health context, not as a pass result for any project-specific phase gate.

## Build evidence

### BUILD-20261023-001 - Controlled per-core Update Code and Build

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:05 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | Host build only |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH`, incremental |
| Baseline Git commit | `028cc8c0657381f1ebf2f29e72cf44b0cddceca9` |
| Projects | `FreeRTOS_Test_SW_M7_0_0`, `FreeRTOS_Test_SW_M7_0_2` |
| Procedure | Select each project; execute **Update Code and Build** |
| Core 0 result | 0 errors, 0 warnings; text 44,088; data 260; bss 10,376; total 54,724 bytes |
| Core 2 result | 0 errors, 0 warnings; text 35,808; data 260; bss 10,376; total 46,444 bytes |
| Configuration result | Both `.mex` files use Clocks tool 19.0 and differ only by intended core-assignment fields |
| Outcome | PASS (incremental host build only) |
| Limitations | Not a clean rebuild; no flash, boot, debugger, or real-board evidence yet |

### BUILD-20261023-002 - Clean multicore rebuild

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:09-20:10 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | Host build only |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH`, clean rebuild followed by per-project Update Code and Build/build |
| Baseline Git commit | `f4135a7` |
| Projects | `FreeRTOS_Test_SW_M7_0_0`, `FreeRTOS_Test_SW_M7_0_2` |
| Core 0 artifacts | 54 object files; ELF 1,679,964 bytes; text 44,088; data 260; bss 10,376; total 54,724 bytes |
| Core 2 artifacts | 54 object files; ELF 1,679,720 bytes; text 35,808; data 260; bss 10,376; total 46,444 bytes |
| Errors | 0; both ELF and map files were regenerated after the clean |
| Warning review | Core 0 full-build summary: 28 compiler warnings from FreeMASTER `freemaster.h` because `FMSTR_PLATFORM_CORTEX_M` and `FMSTR_PLATFORM_S12Z` are tested while undefined under `-Wundef`. The linker also printed three enum-ABI diagnostics for `libc_nano.a`/`libgcc.a`, which were not included in Eclipse's warning count. Core 2's retained log is a later incremental 0-error/0-warning run, so its clean-build warning count was not preserved. |
| Outcome | PASS for clean artifact generation; warning disposition OPEN; hardware gate not exercised |
| Follow-up | Preserve full logs for both cores on the next clean qualification build; investigate warning remediation without editing generated or SDK-owned sources; flash and debug both images on the board. |

## Hardware evidence

### RUN-20261023-001 - H0-A primary-core debug smoke

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:16 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Onboard PEmicro/OpenSDA path selected as `USB1`; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | `c860dee` documentation state; firmware artifacts from clean build `BUILD-20261023-002` |
| Core(s) | `M7_0_0` application core with `CM7_1` lockstep checker |
| Test ID(s) | `H0-A` |
| Procedure/command | Launch `FreeRTOS_Test_SW_M7_0_0_Debug_FLASH_PNE`, then run the generated placeholder application |
| Expected result/tolerance | PEmicro connects, reset/startup script completes, application reaches `main()`, and the core runs the intentional infinite loop |
| Actual result | PEmicro debug session active; DAP successfully powered; reset script completed without a visible error; Debug view reports `Thread #1 (Running : User Request)` and the source/disassembly views identify the loop in `main()`. The TAD session log records a breakpoint hit, multiple step/resume transitions, and normal session termination. |
| Outcome | `PASS` for primary-core connect/flash/run smoke only |
| Evidence paths | `evidence/H0/2026-10-23/core0-running-main-loop.png` (SHA-256 `B4F8682A2DF765C9423F37A9405BE69E51D4FF541498DD03DD195502BBB4C060`); `evidence/H0/2026-10-23/core0-debug-session-tad.log` (SHA-256 `108E705C8A5AD92D2AAA7301F1C58AC3BB5B419CF9E4D5ECC2AE746CDE1591A7`) |
| Deviations/issues | The screenshot was captured after resume and the TAD log does not record source locations, so `exit_code == 0`, reset/reflash repeatability, and the `M7_0_2` group session remain unproven. S32DS migrated the core-0 launch schema and persisted the working `USB1` probe selection. |
| Follow-up | Suspend and inspect the primary core; restart and confirm stop at `main()`; then exercise the generated `Debug_FLASH` group launch |

### RUN-20261023-002 - H0-B multicore group launch

| Field | Value |
|---|---|
| Date/time/timezone | 2026-10-23 20:20-20:21 Europe/Rome |
| Operator | Antonio Ragazzo |
| Target | `HARDWARE` |
| Board/model revision | FRDM-A-S32K358; PCB revision not yet recorded |
| Probe and firmware | Onboard PEmicro/OpenSDA path selected as `USB1`; exact firmware version not yet recorded |
| S32DS version | 3.6.5 |
| RTD/SDK version | PlatformSDK/RTD 7.0.0 |
| Compiler/version | NXP GCC Arm bare metal 10.2 |
| Build configuration | `Debug_FLASH` |
| Git commit/build ID | `8d06720` documentation state; firmware artifacts from clean build `BUILD-20261023-002` |
| Core(s) | `M7_0_0` application core with `CM7_1` lockstep checker, plus independent `M7_0_2` |
| Test ID(s) | `H0-B` |
| Procedure/command | Launch `FreeRTOS_Test_SW_Debug_FLASH_PNE_group`; allow both generated PEmicro child launches to connect and stop |
| Expected result/tolerance | Both ELF images are associated with their intended cores and both debugger sessions stop in their respective `main()` functions without attach/reset/flash errors |
| Actual result | Group launch created `FreeRTOS_Test_SW_M7_0_0.elf` and `FreeRTOS_Test_SW_M7_0_2.elf` sessions. Both `Thread #1` entries were suspended in `main()` after stepping, at `0x0040265e` and `0x00700602` respectively. TAD logs record both DSF sessions and normal termination. The operator had also repeated the primary-core flash/reset/halt check successfully immediately before this run. |
| Outcome | `PASS` |
| Evidence paths | `evidence/H0/2026-10-23/multicore-group-halted-at-main.png` (SHA-256 `4228FABBC16B087EDF247945F73C0241C6A06290C9D4CF5BB7585A2047B5E45D`); `multicore-group-tad-main.log` (SHA-256 `A88F92EF1DA8ABB8BCA2B636DD20D0303FF5292B31F7486E12F5D68AC4929DC3`); `multicore-group-tad-secondary-end.log` (SHA-256 `C8A8224B662C96196AB2A3FF39FBBC5C0322189A2216827E94050F3F121CB0B3`) |
| Deviations/issues | Exact board PCB revision and probe firmware version remain to be recorded. Build warnings from `BUILD-20261023-002` remain open for remediation, but no compiler/linker errors or runtime-debug failure occurred. |
| Follow-up | Begin Phase 1 on the lockstep application core; keep `M7_0_2` parked except when deliberately testing multicore behavior. |

Copy the template below for each executed test. Do not overwrite earlier evidence; append a new run.

## Run template

### RUN-YYYYMMDD-NNN - `<test ID or phase gate>`

| Field | Value |
|---|---|
| Date/time/timezone | |
| Operator | |
| Target | `HARDWARE` / `QEMU` / `ASIC_MODEL` |
| Board/model revision | |
| Board setup/jumpers | |
| Power source/cable | |
| Probe and firmware | |
| S32DS version | |
| RTD/SDK version | |
| FreeRTOS version | |
| Compiler/version | |
| Build configuration | |
| Git commit/build ID | |
| Core(s) | |
| Clock configuration | |
| Cache/MPU state | |
| Test ID(s) | |
| Preconditions | |
| Procedure/command | |
| Expected result/tolerance | |
| Actual result | |
| Outcome | `PASS` / `FAIL` / `SKIP` / `ERROR` / `TIMEOUT` |
| Evidence paths | |
| Deviations/issues | |
| Follow-up | |

### Evidence checklist

- build console log;
- ELF and map-file identity/size;
- flash/debug console result;
- raw UART/test-protocol transcript;
- scope, logic analyzer, CAN/LIN analyzer, or packet capture where relevant;
- board photo/setup diagram when wiring changes;
- QEMU command line and model revision for model runs;
- allowed-difference explanation for parity judgments.

## Phase-gate summary

| Gate | Status | Latest run | Notes |
|---|---|---|---|
| H0 Reproducible baseline | PASS | RUN-20261023-002 | Clean build artifacts, repeatable primary flash/reset/halt, and multicore group launch verified on hardware |
| H1 Bare-metal observability | PLANNED | - | |
| H2 FreeRTOS baseline | PLANNED | - | |
| H3 Test harness | PLANNED | - | |
| H4 GPIO/interrupts | PLANNED | - | |
| H5 Timers/timebase | PLANNED | - | |
| H6 LPUART stress | PLANNED | - | |
| H7 ADC/I2C | PLANNED | - | |
| H8 LPSPI/eDMA | PLANNED | - | |
| H9 CAN/LIN | PLANNED | - | |
| H10 Ethernet/storage/memory | PLANNED | - | |
| H11 Multicore | PLANNED | - | |
| H12 Reset/power/safety | PLANNED | - | |
| Parity regression | PLANNED | - | |
