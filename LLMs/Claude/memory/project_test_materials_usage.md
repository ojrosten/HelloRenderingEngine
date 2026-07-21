---
name: avocet-test-materials-usage
description: How HelloRenderingEngine uses sequoia test materials — flat consumption-only mode so far; Prediction/seqpat machinery unused as yet
metadata:
  type: project
---

Full mechanics: [[sequoia-test-materials]]. As of 2026-07-20, avocet uses **only the flat, consumption-only mode**: every `TestMaterials/` leaf holds read-only inputs; there are no `Prediction` directories and no `.seqpat` files anywhere in the project. The generate-compare-update workflow (WorkingCopy/Prediction/Auxiliary + `update-materials`) is not (yet?) exercised here — tacitly accepted by the user, not an oversight.

Current examples:
- `TestMaterials/OpenGL/Resources/ShaderProgramFreeTest` (and the other `ShaderProgram*` siblings) — shader sources; tests grab them via `shaderDir{working_materials()}`.
- `TestMaterials/OpenGL/Geometry/PolygonFreeTest/Shaders` — accessed through a `get_shader_dir()` helper (`working_materials() / "Shaders"`).
- `TestMaterials/Core/AssetManagement/UniqueImage*` — PNGs (and a deliberate `not_an_image.txt`) for image-loading tests, including absent-file paths built on `working_materials()`.
- `TestMaterials/TestFramework/OpenGL/LabellingTestDiagnostics`, `TestMaterials/OpenGL/Capabilities/CapabilityManagerFreeTest` — same pattern.

**Why:** knowing the ceiling of current usage stops me mis-describing the project ("uses seqpat" — it doesn't) and marks the headroom: rendered-output or generated-file comparisons (e.g. screenshot/framebuffer dumps, serialized state) would be natural first customers for `Prediction` + `.seqpat` here.

**How to apply:** when a test needs materials, mirror the test-source path under `TestMaterials/`; flat mode needs nothing more. If a future test generates files for comparison, introduce a `Prediction` dir (this flips the layout — existing loose files would then need to move into `WorkingCopy`) and remember whole-tree comparisons need `weak_equivalence`. Note: materials count as prune dependencies, so touching them re-runs the test.
