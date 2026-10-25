# Build and debug guide

This guide covers the current `Debug_FLASH` AMP workflow. It is based on tracked project/launch configuration and the recorded board runs; it does not imply a new live IDE or board verification.

## Recorded tool baseline

| Component | Version |
|---|---|
| S32 Design Studio for S32 Platform | 3.6.5 |
| S32K3 PlatformSDK / RTD | 7.0.0 |
| NXP GCC for Arm bare metal | 10.2 |
| FreeRTOS | V11.1.0, Cortex-M7 `r0p1` port, heap-1 |
| S32 Configuration Tools | `.mex` schema 19; Clocks document version 19.0 |
| PEmicro plug-in | `com.pemicro.debug.gdbjtag.pne_6.2.7.202610202056` |

FreeMASTER is deferred and is not an active runtime dependency. Board PCB revision and exact probe firmware are still missing from the recorded baseline.

## Workspace and build

1. Clone the repository and use a fresh S32DS workspace; do not copy another workspace's `.metadata`.
2. Import both core projects while preserving the directory relationship with `FreeRTOS_Test_SW/common`. The optional non-building `FreeRTOS_Test_SW_Common` project exposes shared files for browsing. Both core projects already compile `../common/src` through their `common_src` linked folder and include `../common/include`.
3. Resolve the recorded SDK/toolchain dependencies. Validate/regenerate both `.mex` configurations through S32DS/S32 Configuration Tools and review the generated diff before building. Project-owned application sources are edited separately from generated files.
4. Build both projects in `Debug_FLASH`. Preserve complete compiler/linker logs and identify both ELF/map pairs. Red indexer markers and compiler diagnostics must be evaluated separately.
5. A successful build establishes artifact generation. Flashing, board behavior, and QEMU parity each require their own evidence in [TEST_LOG.md](TEST_LOG.md).

Other managed-build configurations exist, but the tracked and hardware-exercised debug workflow is FLASH. Their presence alone does not establish a RAM/Release debug qualification.

## Tracked debug launch roles

The launch files under each project's `Project_Settings/Debugger` define the following roles. These are configuration-file observations, not assumed GUI field names.

| Launch | Role |
|---|---|
| `FreeRTOS_Test_SW_Debug_FLASH_PNE_group` | Starts the primary child followed by the secondary child. |
| `FreeRTOS_Test_SW_M7_0_0_Debug_FLASH_PNE` | Loads primary symbols/image and programs the secondary ELF as its additional object file. |
| `FreeRTOS_Test_SW_M7_0_2_Debug_FLASH_PNE` | Loads secondary symbols with image loading disabled; relies on the primary launch to program that image. |

The group itself is a coordinator. Programming both images comes from the primary child's explicit additional-ELF configuration. Keep that division intact when recreating launches. Firmware CM7_0 owns MC_ME release after global initialization; a debugger must not independently start CM7_2 around that sequence. The [architecture](AMP_FREERTOS_ARCHITECTURE.md) describes the boot contract.

## PEmicro macro workaround

For the recorded plug-in version, the stock `supportFiles_ARM/NXP/S32K3xx/S32K358.mac` conflicts with application-owned MC_ME startup. The hardware-proven host workaround renames it to `S32K358_.mac`, preventing automatic execution. The inspected host location is:

```text
C:/NXP/S32DS.3.6.5/eclipse/plugins/com.pemicro.debug.gdbjtag.pne_6.2.7.202610202056/supportFiles_ARM/NXP/S32K3xx/S32K358_.mac
```

The stock macro rewrites boot/control state and had also initialized SRAM. Firmware now owns the required SRAM ECC initialization. A repository checkout or release archive does not apply the host rename; check its state after IDE/plug-in updates. See [D-023](DECISIONS.md#d-023---pemicro-startup-macro-must-not-own-application-mc_me-state) and `RUN-20261025-014/015` for the inspection and board evidence.

## STM freeze during debug

Both current `.mex` configurations enable `StmFreezeEnable`. The generated firmware configuration causes `Stm_Ip_Init()` to program the STM hardware debug-freeze bit so its counter stops during debugger halt. This is a peripheral configuration, not a launch checkbox.

Use coordinated multicore group debugging. The historical primary-only diagnostic launch could hold the other core halted and freeze STM unexpectedly; the later halt/resume task issue was resolved by enabling STM freeze. See [D-021](DECISIONS.md#d-021---multicore-debug-launch-policy-and-stm-debug-freeze). Package `v0.1.0` retains the earlier freeze-disabled configuration; `v0.2.0` enables it on both instances. Consult the specific package's configuration when reproducing an older run.

## Component testing and packaging

After building/debugging the application, follow the component-specific guide, beginning with [STM](test_sw/STM.md). Record the actual build and board result separately. Create new release packages with the manual workflow in [RELEASES.md](RELEASES.md).
