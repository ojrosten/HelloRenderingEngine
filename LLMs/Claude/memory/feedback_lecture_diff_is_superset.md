---
name: feedback-lecture-diff-is-superset
description: A delivered lecture branch's diff is a superset of the lecture's actual content — incidental prep-time bug fixes and toolchain-forced churn ride along and weren't necessarily taught.
metadata:
  type: feedback
---

When reviewing "the content of lecture X", remember that the `end_lecture_<X-1>..end_lecture_X` diff is **not** identical to what was taught. The branch accumulates everything committed during prep, which includes things outside the lecture's pedagogical scope:

- **Incidental bug fixes found during prep.** While writing tests for lecture 48, the user discovered the `discrete_extent::size()` 32-bit-multiply overflow, fixed it (`size()` → `area()`, widening with `std::uint64_t{width} * height`), and *notified attendees* — but it wasn't planned lecture material.
- **Toolchain-forced churn.** The `STD_GENERATOR` macro removal was forced by an MSVC update (18.6.2 shipping `<generator>`); the user then simplified further but deliberately did **not** alert the audience, considering it too minor and fully captured in the commit record.

So lecture 48's actual taught content was narrower than its diff: the conversion utilities (`checked_conversion_to`, `to_gl_underlying_value` / `std::to_underlying`) and the testing-vs-dependencies lesson they enable (see [[feedback-testing-vs-dependencies-tradeoff]]).

**How to apply:** When asked to review a lecture's *content*, review the whole diff for correctness regardless — but don't assume every change is "the lecture," and don't frame incidental/forced changes as if they were pedagogical decisions. If scope matters to the assessment, ask which changes were lecture material vs. ride-alongs, or attribute tentatively. Relates to [[reference-lecture-branch-naming]] (branch *stages*) and [[feedback-suboptimal-not-always-intentional]] (flag-as-question rather than pre-decide).
