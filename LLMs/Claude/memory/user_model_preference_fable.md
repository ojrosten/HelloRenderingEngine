---
name: user-model-preference-fable
description: User switched to Fable 5 (2026-07-10) and wants to be told if a session runs on Opus 4.8 or another model instead
metadata: 
  node_type: memory
  type: user
  originSessionId: 856f9430-1fe6-4ebf-8adb-06c5fe5d5bc0
---

On 2026-07-10 the user switched their default model to Fable 5 (`claude-fable-5`) and is evaluating how it performs. They explicitly asked to be told if the model ever reverts to Opus 4.8.

**How to apply:** At session start, check the "You are powered by" line in the system prompt. If it names anything other than Fable 5, mention it to the user early in the session.
