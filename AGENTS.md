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
- Keep the root README as an overview, release table, and documentation index. Put testSW usage/configuration/results in one guide per component under `docs/test_sw/`; update the README link when adding a component (D-028).

Current status (2026-10-25): H0 and the dual-core AMP runtime sub-gates have passed. Both independent FreeRTOS V11.1.0 images compile the common support sources; `RUN-20261025-017` confirms protocol-V3 `READY` states and zero fatal errors on both cores. MC_ME release is the CM7_2 boot gate; the shared status object is diagnostic, with no `SYSTEM_READY` polling or `WAITING` state. `STM-001..004` have functionally passed on STM0_CH3 and STM2_CH3 (`RUN-20261025-018/019/020`), and `v0.2.0` packages this first STM test implementation. Release archives contain ELF/map pairs and manifests; historical run-to-binary identity gaps remain explicit. Current tasks still use dynamic heap-1 allocation, and CM7_2 Idle/malloc/stack-overflow hook parity is open. Combined H1/2 and H5 remain in progress: standalone cold boot, failure/warm-reset tests, observability, soak evidence, remaining timer coverage, and deterministic QEMU parity are not yet qualified. Physical jitter measurement is not a release gate (D-027). Read the living documents for detailed next steps rather than repeating historical mailbox work.
