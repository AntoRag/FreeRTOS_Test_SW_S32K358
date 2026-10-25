# S32K358 FreeRTOS peripheral-test software

Peripheral test software for the NXP **FRDM-A-S32K358** board. The project establishes real-board behavior as the reference for running the same tests on QEMU and future ASIC models.

The application uses AMP: two independently linked images, each with its own single-core FreeRTOS instance. Test components are added incrementally, with their scope, usage, and evidence documented separately.

## Releases

| Release | What it introduces | Package |
|---|---|---|
| **v0.2.0** | First STM peripheral testSW component on both AMP cores. See the [STM guide](docs/test_sw/STM.md). | [Debug_FLASH .7z](release/S32K358_FreeRTOS_v0.2.0_Debug_FLASH.7z) |
| **v0.1.0** | Initial dual-core AMP FreeRTOS runtime, common support code, diagnostics, shared startup status, and release packaging. | [Debug_FLASH .7z](release/S32K358_FreeRTOS_v0.1.0_Debug_FLASH.7z) |

Packages include source code, prebuilt firmware for both images, debugger launch files, and an artifact manifest. [Release records](docs/RELEASES.md) describe provenance, contents, and packaging instructions.

The current STM component has functional passes on the physical board. Broader coverage and QEMU parity remain open; see the [roadmap](docs/ROADMAP.md) and [test log](docs/TEST_LOG.md) for the evidence and remaining gates.

## Test software components

| Component | Guide | Introduced in |
|---|---|---|
| STM | [Scope, configuration, execution, and result interpretation](docs/test_sw/STM.md) | v0.2.0 |

Each future component should have its own guide linked here.

## Documentation

| Document | Purpose |
|---|---|
| [Build and debug](docs/BUILD_AND_DEBUG.md) | Tool baseline, workspace setup, builds, debug launches, and host workaround. |
| [Project context](docs/PROJECT_CONTEXT.md) | Current baseline, platform facts, and evidence boundaries. |
| [AMP architecture](docs/AMP_FREERTOS_ARCHITECTURE.md) | Initialization ownership, shared support code, task model, and diagnostics. |
| [Roadmap](docs/ROADMAP.md) | Planned capabilities and hardware/model qualification gates. |
| [Test log](docs/TEST_LOG.md) | Dated build, artifact, and real-board results. |
| [Hardware dependencies](docs/PERIPHERALS_USED.md) | Register blocks required by the current firmware and device model. |
| [Engineering rules](docs/ENGINEERING_RULES.md) and [decisions](docs/DECISIONS.md) | Development policy and accepted/proposed design choices. |
| [Releases](docs/RELEASES.md) | Published package records and manual packaging workflow. |
| [References](docs/REFERENCES.md) | External manuals and document identities. |

## Repository layout

- [FreeRTOS_Test_SW/](FreeRTOS_Test_SW/): both application projects and common source code.
- [docs/](docs/): project and component documentation.
- [tools/](tools/): release tooling.
- [release/](release/): retained versioned packages.
- [evidence/](evidence/): archived verification evidence where available.
- [AGENTS.md](AGENTS.md): instructions and context for repository agents.

