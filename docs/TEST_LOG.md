# Hardware and Model Test Log

No phase gate has passed yet. The build-only evidence below does not by itself qualify a hardware-backed phase gate.

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
| H0 Reproducible baseline | IN PROGRESS | BUILD-20261023-002 | Clean artifacts produced with zero errors; warnings reviewed but not yet dispositioned; board execution remains |
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
