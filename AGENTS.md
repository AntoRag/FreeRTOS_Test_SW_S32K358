# S32K358 FreeRTOS Test Software - Agent Guide

This workspace supports a learning-oriented, production-quality test application for the NXP FRDM-A-S32K358. The application will validate S32K358 peripheral behavior on real hardware and provide deterministic comparison workloads for a QEMU S32K358 model.

Before doing project work, read these files in order:

1. `docs/PROJECT_CONTEXT.md`
2. `docs/ROADMAP.md`
3. `docs/ENGINEERING_RULES.md`
4. `docs/DECISIONS.md`
5. `docs/AMP_FREERTOS_ARCHITECTURE.md`
6. `docs/TEST_LOG.md`

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
- Prefer the smallest safe step and one newly introduced variable per board experiment.

Current status: hardware gate H0 has passed. The current two-image baseline is still bare-metal. The accepted next architecture is AMP with one independent single-core FreeRTOS V11.1.0 instance per existing project/ELF; SMP is explicitly rejected. H1A integrates and proves the primary-core kernel before H1B repeats the process on the secondary core.
