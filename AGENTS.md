# S32K358 FreeRTOS Test Software - Agent Guide

This workspace supports a learning-oriented, production-quality test application for the NXP FRDM-A-S32K358. The application will validate S32K358 peripheral behavior on real hardware and provide deterministic comparison workloads for a QEMU S32K358 model.

Before doing project work, read these files in order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/ROADMAP.md`
3. `docs/ENGINEERING_RULES.md`
4. `docs/DECISIONS.md`
5. `docs/AMP_FREERTOS_ARCHITECTURE.md`
6. `docs/TEST_LOG.md`
7. `docs/PERIPHERALS_USED.md`

Operating rules:

- The human user owns application coding and uses S32 Design Studio to edit, generate, build, flash, and debug unless they explicitly delegate an operation.
- Do not modify generated S32 Configuration Tools or RTD files manually. Record the `.mex` change and regenerate through S32DS.
- Treat a successful compiler build, a clean Eclipse index, successful flashing, and a passing hardware test as four different facts.
- A phase is complete only when its exit criteria are met and a real-board result is recorded in `docs/TEST_LOG.md`.
- QEMU parity is an additional gate, not a substitute for real-board evidence.
- Label unverified statements as `Proposed`, `Assumed`, or `Open`; never silently promote them to `Confirmed`.
- Add architectural or naming choices to `docs/DECISIONS.md` before relying on them broadly.
- Preserve exact tool, SDK, RTD, board, probe, build-configuration, and firmware versions in test evidence.
- Keep tests deterministic, independently runnable, bounded by timeouts, and machine-readable.
- Avoid coupling test logic directly to NXP-generated symbols. Use a thin board/support layer so the same test intent can run on hardware and QEMU.
- Update `docs/PERIPHERALS_USED.md` whenever executed firmware introduces or removes a hardware-module dependency.
- Before giving S32DS GUI instructions, verify the exact field names and available controls against the user's installed S32DS version using the applicable skill and direct UI or installed-plugin evidence. Never translate internal `.launch`/`.mex` attribute names into assumed GUI labels. If the UI cannot be inspected, label the mapping `Open` and request a screenshot of the relevant tab instead of guessing.
- Prefer the smallest safe step and one newly introduced variable per board experiment.

Current status: hardware gate H0 has passed. Both AMP images contain independent single-core FreeRTOS V11.1.0 integrations; SMP is explicitly rejected. `RUN-20261025-015`, checkpointed by commit `49b4ad7`, proved simultaneous real-hardware execution of the complete STM0-released `TaskA` set on `M7_0_0` and STM2-released `TaskB` set on `M7_0_2`, with exact interrupt-to-execution count equality and both fatal states at zero. This is a passed dual-core runtime sub-gate, not completion of the combined H1/2 gate. The next smallest steps are CM7_2 mailbox-state publication (`WAITING`/`READY`/`FAILED`), CM7_2 diagnostic-hook parity, and a standalone no-debugger cold-boot test; UART/LED observability, negative-hook tests, soak evidence, and durable build artifacts remain open.
