# Hardware and Model Test Log

No phase or test has passed yet.

User-provided prerequisite: the FRDM-A-S32K358 board is known working and has already been tested. This is accepted as board-health context, not as a pass result for any project-specific phase gate.

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
| H0 Reproducible baseline | PLANNED | - | |
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
