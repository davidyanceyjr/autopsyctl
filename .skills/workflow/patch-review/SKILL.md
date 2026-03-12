# patch-review

Purpose
-------
Review a patch before commit or PR.

Procedure
---------
1. Read `git diff` and `git diff --cached`.
2. Check for unrelated edits, debug prints, dead code, formatting noise, accidental behavior changes, missing tests, and missing docs updates.
3. Assess whether the patch should be split.
4. Summarize whether the patch is ready and what must change first.
