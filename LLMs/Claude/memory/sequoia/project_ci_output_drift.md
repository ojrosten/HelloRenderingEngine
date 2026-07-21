---
name: sequoia-ci-output-drift
description: "Design (2026-07-21) for automatic detection of versioned-output drift on CI — platform-dependence defects that manifest as git diffs, not test failures; discriminators as the desired/undesired classifier"
metadata: 
  node_type: memory
  type: project
  originSessionId: f58cc83f-8059-4d66-a2ba-ed07ba4821c1
  modified: 2026-07-21T11:30:37.220Z
---

Raised by the user 2026-07-21, sparked by the E/F-series findings: some versioned-output "failures" never fail a test — they manifest as git diffs in `output/DiagnosticsOutput` / `output/TestSummaries` (`output/Prune` is gitignored, out of scope). Locally a human referees desired diffs (intended output changes, reviewed and committed) from undesired ones (platform-dependence leaks). On GitHub Actions there is no referee. The subtlety the user named: any automatic mechanism must distinguish the two.

**Key insight — the distinction is already encoded twice:**

- *Temporally*: desired diffs are a pre-push phenomenon (run → review diff → commit). A post-clone CI run should end **byte-clean**; any post-run dirt is a defect or a stale fork.
- *Spatially*: the discriminator system ([[avocet-discriminator-axes]]) is a machine-readable declaration of which files are *allowed* to vary by platform. Classify post-run dirt:
  - **(a) shared (undiscriminated) file changed** → platform-dependence leak (the E/F mole classes) → hard fail;
  - **(b) discriminated file matching the runner's own platform changed** → a fork the pushing developer *couldn't* have regenerated (e.g. Windows dev changes exception text; the `_Linux_llvmpipe` fork is necessarily stale) → not a leak — harvest it;
  - **(c) new untracked discriminated file** → first run on a platform combination with no committed fork → harvest.

**Mechanism ladder** (each step subsumes the previous):

1. Zero sequoia changes: `git diff --exit-code -- output/` as a post-run workflow step. Blunt — no (a)/(b) distinction.
2. Workflow-level classification: parse `_<disc>.txt` suffixes; the runner's own discriminator is available from `Setup.txt` (written beside the executable by TestAllMain).
3. **Sequoia-native (recommended)**: the runner already knows every file it writes (`m_FilesWrittenTo`) and each test's `output_discriminator`/`summary_discriminator`. Add a post-run **drift report**: before each versioned write, byte-compare with existing content; classify changed files into shared / own-discriminated / new; under an opt-in CI flag, exit nonzero on shared drift and list harvestables. No git dependency, exact comparison, and doubles as a local pre-push check.

**Prerequisites & subtleties:**

- **E4 (binary writes) is a prerequisite for Windows runners**: actions/checkout leaves an LF tree; text-mode CRLF rewrites would make every file "drift" — noise drowns signal. Linux runners are usable immediately.
- Determinism assumption: shared files must be run-deterministic; nondeterminism surfaces as flaky drift — itself worth catching (instability analysis covers check-level flakiness; this would catch output-level).
- **Harvest path**: upload category (b)/(c) files as workflow artifacts (optionally a bot PR). This elevates CI from verifier to *fork maintainer* for CI-covered platforms (the llvmpipe leg of the parked CI sketch) — attacking the current burden of needing physical access to N platform/GPU combinations to refresh forks.
- Complementary to the F-strategy parity battery: the battery is the in-lab detector for type-name moles inside sequoia's own suite; drift detection is the field-wide net in client repos (paths, driver text, check counts — everything).

Status: design recorded, not started; summarized in `LLMs/Claude/SEQUOIA_TOOLING.md` §5. Sequoia-side runner feature + thin Actions policy on the avocet side.
