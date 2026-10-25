# Hardware Modules Required by the Current Software

This is the living inventory of hardware register blocks that the current firmware needs in order to execute. It is the starting scope for the S32K358 QEMU model.

Inclusion rule: add a module when executed startup, RTD, kernel, or application code accesses its registers or depends on its interrupt/exception behavior. Do not add a peripheral merely because its driver is installed, its header is included, its clock gate appears in the generated clock configuration, or unused code is present in the SDK.

The table is the union of the two AMP images. Core ownership is intentionally omitted because a hardware block required by either image is still required by the device model. Update this file in the same change that introduces or removes a hardware dependency.

| Exact hardware module | How the current software uses it |
|---|---|
| `MC_ME` | `Clock_Ip_Init()` uses partition and clock-gating controls. The CM7_0-owned Power mode transition programs the CM7_2 (`CORE4`) boot address `0x00700000`, enables its clock, and releases it. |
| `MC_RGM` | `Power_Ip_Init()` initializes reset-generation policy/status registers because `POWER_IP_DISABLE_RGM_INIT` is generated as `STD_OFF`. The Power mode path also uses MC_RGM mode/reset-domain support. |
| `PMC` | `Power_Ip_Init()` initializes the Power Management Controller digital interface because `POWER_IP_DISABLE_PMC_INIT` is generated as `STD_OFF`. |
| `DCM_GPR` | `Power_Ip_Init()` disables global pad keeping through the Power driver's DCM_GPR path. This is distinct from `CONFIGURATION_GPR`. |
| `MSCM` | Startup reads the executing-core and TCM configuration fields and programs interrupt routing through `IRSPRC`. |
| `SCB` (Arm Cortex-M7 System Control Block) | Startup configures FPU access and instruction/data caches. FreeRTOS uses SCB system-handler priority and pending-exception registers for SVC, PendSV, and SysTick scheduling behavior. |
| `MPU` (Arm Cortex-M7 Memory Protection Unit) | The S32K3 startup code programs and enables the project-level memory map. The FreeRTOS task-level MPU port is currently disabled, but the startup MPU remains required. |
| `NVIC` (Arm Cortex-M7 Nested Vectored Interrupt Controller) | Each core initializes its local interrupt context. It installs, prioritizes, enables, and dispatches `STM0_IRQn` on CM7_0 and `STM2_IRQn` on CM7_2. FreeRTOS also checks interrupt-priority behavior for ISR-safe kernel calls. |
| `SysTick` (Arm Cortex-M7 system timer) | Provides the 1 kHz FreeRTOS kernel tick. It drives kernel time and scheduling services, but not the application 10/100/1000 ms releases. |
| `FIRC` | Supplies the configured 48 MHz internal clock and the source selected for the STM clock path; `Clock_Ip_Init()` also programs its standby setting and FIRC divider support. |
| `SIRC` | `Clock_Ip_Init()` programs the SIRC standby-enable field to the generated disabled state. It is not currently selected as an application timebase. |
| `FXOSC` | Enables and monitors the 16 MHz external crystal used as the reference for the system PLLs. |
| `SXOSC` | Enables the 32.768 kHz external crystal selected by the generated RTC clock mux configuration. |
| `PLLDIG` (`IP_PLL`) | Generates the configured high-speed system clock domains from FXOSC. |
| `PLLDIG_AUX` (`IP_PLL_AUX`) | Generates the configured auxiliary PLL clock outputs from FXOSC. |
| `MC_CGM` | Programs system and peripheral clock multiplexers, dividers, and the common divider update, including the clock paths used by STM0 and STM2. |
| `CONFIGURATION_GPR` | The clock driver reads the application-access policy and, when permitted, programs the FIRC divider-selection field. |
| `RTC` | The clock driver writes `RTC.RTCC.CLKSEL` to select SXOSC. The RTC counter/alarm functionality is not otherwise used. |
| `CMU_FC_0` | Receives the generated FXOSC monitor configuration. The monitor is currently configured disabled, but its register block is touched during clock initialization. |
| `CMU_FC_3` | Receives the generated core-clock monitor configuration. The monitor is currently configured disabled, but its register block is touched during clock initialization. |
| `CMU_FC_4` | Receives the generated AIPS-platform-clock monitor configuration. The monitor is currently configured disabled, but its register block is touched during clock initialization. |
| `FLASH` / `C40ASF` (`IP_FLASH`) | `Clock_Ip_Init()` programs flash read wait states for the configured core frequency before normal execution continues. |
| `PRAMC_0` | `Clock_Ip_Init()` programs the RAM-controller wait-state/flow-through behavior for the configured frequencies. |
| `PRAMC_1` | `Clock_Ip_Init()` programs the RAM-controller wait-state/flow-through behavior for the configured frequencies. |
| `PRAMC_2` | `Clock_Ip_Init()` programs the RAM-controller wait-state/flow-through behavior for the configured frequencies. |
| `STM_0` | Channels 0, 1, and 2 run in continuous compare mode at the configured 1 MHz counter rate and notify the three CM7_0 TaskA rate classes. Channel 3 (`Core0StmTestChannel`) is reserved for debugger-selected one-shot/continuous STM characterization, counter monotonicity, and compare readback tests. |
| `STM_2` | Channels 0, 1, and 2 run in continuous compare mode at the configured 1 MHz counter rate and notify the three CM7_2 TaskB rate classes. Channel 3 (`Core2StmTestChannel`) is reserved for the same local STM characterization tests on CM7_2. |

## Explicitly not in the current inventory

- `FreeMASTER`: removed/deferred; no FreeMASTER service is initialized or polled.
- `STM_1`: not used by the current runtime.
- `eMIOS`, `PIT`, `LPUART`, `FlexCAN`, `SIUL2`, ADC, SPI, I2C, Ethernet, and DMA blocks: not yet accessed by the application runtime. Add each exact instance when its first test is introduced.
- Peripherals whose clocks are only gated by the broad generated `Clock_Ip` configuration: clock-gate programming requires `MC_ME`/`MC_CGM`, not a functional model of every gated peripheral.

## Evidence basis

Current entries were derived from both application images, generated S32 Configuration Tools data, linked `Debug_FLASH` maps, S32K358 device headers, startup code, executed RTD clock/power/interrupt/STM paths, and the FreeRTOS Cortex-M7 port. The channel-3 STM test configuration and common runner have passed functional checks on both core-owned instances: STM0_CH3 on CM7_0 and STM2_CH3 on CM7_2 (`RUN-20261025-018` through `RUN-20261025-020`). These tests use already-required STM0/STM2 modules and introduce no new module dependency. Build identity, archived ELF/map files, standalone cold boot, and QEMU parity remain separate evidence items recorded in `docs/TEST_LOG.md`.
