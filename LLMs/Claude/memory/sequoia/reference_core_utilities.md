---
name: sequoia-core-utilities
description: Map of sequoia's Core (Object/Meta/DataStructures/Concurrency/Logic) and small utility modules, with factory details relevant to the reflection plan
metadata:
  type: reference
---

Survey of `dependencies/sequoia/Source/sequoia/` outside TestFramework and Maths/Physics (as of 2026-07). Namespaces: `sequoia`, `sequoia::object`, `sequoia::utilities`, `sequoia::data_structures`, `sequoia::meta`, `sequoia::concurrency`, `sequoia::runtime`.

**Core/Object — the factory** (`Core/Object/Factory.hpp`), centerpiece of the planned reflection-based test registration ([[sequoia-roadmap]]):
- `factory<Products...>` — products fixed at compile time, keys are runtime `std::string`s. Storage is a name-sorted `std::array` of `(string, variant<producer...>)`; lookup by binary search; duplicate/empty names throw at construction.
- `make(name, args...) -> std::variant<Products...>`; `make_or<Default>` falls back instead of throwing. Constraint: **every** product must be brace-initializable from the same argument list (`initializable_from` — this homogeneity is the "bit of tweaking" the reflection plan will likely need to address).
- Names: explicit at construction, or auto via `nomenclator<T>` specializations (`has_extrinsic_nomenclator` enables the factory's default ctor) — the natural customization point for reflection-derived names.
- Support: `Creator.hpp` (`producer`, `direct_forwarder`), `Handlers.hpp` (`by_value<T>` vs `shared<T>` storage models, used by the graph library).
- `Suite.hpp`: `suite<Ts...>` — named, nestable heterogeneous collections; `extract_leaves` (vector-of-variant), `extract_tree` (builds a `maths::directed_tree` mirroring suite structure), `granular_filter` (select by suite name and/or item key, tracking unmatched selections). This is the test-suite grouping/selection layer that sits naturally atop the factory.

**Core/Meta** — the metaprogramming vocabulary:
- `Concepts.hpp`: `initializable_from` (braced-init analogue of `constructible_from`, pervasive), `invocable_r`, `pseudoregular`, `moveonly`, `serializable_to`, `deep_equality_comparable`, `faithful_range`.
- `TypeTraits.hpp` (2018-era): `resolve_to_copy` (perfect-forwarding-ctor guard), deep-equality machinery working around under-constrained STL `operator==`.
- `TypeAlgorithms.hpp` (2025, `sequoia::meta`): consteval `type_name<T>()` via `std::source_location`; full compile-time algorithm suite over type packs — `stable_sort` (merge sort of type lists ordered by type name!), `filter`, `merge`, `flatten`, etc. Clearly groundwork pointing toward the reflection plans.
- `TypeList.hpp` (2024), `Sequences.hpp` (index-sequence algebra), `Utilities.hpp` (`overloaded`, `invoke_with_specified_args`).

**Core/DataStructures** — partitioned sequences back the graph library's edge storage:
- `bucketed_sequence` (vector-of-vectors), `partitioned_sequence` (single contiguous buffer + monotonic-sequence offsets), `static_partitioned_sequence` (constexpr, over `std::array`). API: `add_slot`, `push_back_to_partition`, `begin_partition(i)`, `partition_range`. Deeply allocator-aware (POCCA/POCMA/POCS honored via `assignment_helper`).
- `mem_ordered_tuple` (2024): declaration-order layout, C++26 pack indexing behind feature-test macro ("TO DO: restore … once MSVC supports it").
- Constexpr `static_stack`/`static_queue`/`static_priority_queue`.
- `ContainerUtilities/Iterator.hpp`: policy-based `utilities::iterator` (dereference policy may carry auxiliary data, e.g. partition index); `ArrayUtilities.hpp` `to_array` (checked, transforming, constexpr).

**Core/Concurrency** (`ConcurrencyModels.hpp`, 2018): interchangeable `serial<R>` / `asynchronous<R>` / `thread_pool<R, MultiPipeline>` sharing a `push(fn, args...)` interface; thread pool has task stealing. Pre-`jthread` (manual join) — one of the clearer older-era corners.

**Core/Logic** (`Bitmask.hpp`): opt-in bitmask enums — specialize `as_bitmask<Enum>`, expose operators in client namespaces via `NAMESPACE_SEQUOIA_AS_BITMASK`.

**One-liners**: `Algorithms/` — constexpr heap/stable sort built on `ranges::iter_swap` so behavior is customizable by overloading `iter_swap` (exploited to sort graph nodes), plus `cluster`. `TextProcessing/` — `indentation` type + `camel_to_words`/`to_snake_case` etc., built for test-output formatting and test generation. `Streaming/` — `read_to_string` (optional-returning), `write_to_file`, `read_modify_write`. `FileSystem/` — `normal_path` (lexically-normalized path wrapper). `Runtime/ShellCommands` — composable `shell_command` with `&&` chaining and redirection; drives builds of generated projects in the test framework. `PlatformSpecific/` — compiler discriminators, `SEQUOIA_NO_UNIQUE_ADDRESS`, a shim for libc++'s missing parallel algorithms (libc++ is treated as a support floor).

**Era/style calibration**: continuously modernized since 2018 — even old files now use concepts/ranges/CTAD; age shows structurally (recursive TMP, `std::thread`, trait triples alongside newer requires-expression traits), not syntactically. House conventions: `m_PascalCase`, brace-init, hidden friends, enum-classes over bools. Minor cosmetic typos persist in older files (`transfomer_type` in Creator.hpp, "Insuffucient" in ArrayUtilities.hpp, "descriminate" in PlatformDiscriminators.hpp).
