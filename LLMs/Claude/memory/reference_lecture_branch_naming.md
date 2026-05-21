---
name: reference-lecture-branch-naming
description: Lecture-development branches use end_lecture_X-{experimental,preliminary,expected} suffixes that mark refinement stage.
metadata:
  type: reference
---

While developing a new lecture's code, the user uses a three-stage branch naming convention. Order reflects increasing refinement:

1. **`end_lecture_X-experimental`** — earliest draft. Most rough. Ideas are still being tried out; structure is likely to change.
2. **`end_lecture_X-preliminary`** — refined draft. Major direction settled, details still in flux.
3. **`end_lecture_X-expected`** — most refined. Close to what will actually be presented in the lecture.

Once the lecture is delivered, the branch lands as `end_lecture_X` (no suffix), which is the stable per-lecture anchor (see [[user-teaching-project]]).

**How to apply:** When reviewing or diffing one of these branches:

- **Diff base:** the prior lecture's stable branch is usually right — e.g. for `end_lecture_47-experimental`, diff against `end_lecture_46`. If iterating *within* the same lecture (e.g. `-preliminary` vs `-experimental`), diff between those two to see what was tightened.
- **Review register:** match the branch stage.
  - `-experimental`: structural ideas welcome, sub-optimal code is likely work-in-progress, feedback can be broad. Don't fixate on polish.
  - `-preliminary`: direction is settled — focus on "what's missing" or "what would I be uncomfortable presenting like this," not on rearchitecting.
  - `-expected`: tight, high-confidence feedback only. The user is close to recording/presenting; this is the wrong moment to introduce new ideas.

If the branch name doesn't carry a suffix, treat it as already-delivered stable content.