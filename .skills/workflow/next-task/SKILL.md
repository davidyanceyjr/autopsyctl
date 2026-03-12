# next-task

Purpose
-------
Identify the next correct task in the workflow.

Procedure
---------
1. Use the same inspection approach as `repo-state`.
2. Prefer the smallest forward-moving task.
3. Do not recommend coding if spec or design is missing.
4. Do not recommend commit if tests or patch review are missing.
5. Return one next task, one skill to use, and one reason.

Decision Rules
--------------
- No issue: create or refine issue
- Issue but no spec: `issue-to-spec`
- Spec but weak or missing design: `design-review`
- Design but no coding plan: `implementation-plan`
- Implementation but no tests: `test-generator`
- Failing tests: `repro` or `c-triage`
- Passing tests with diff: `patch-review`
- Behavior changed: `docs-sync`
