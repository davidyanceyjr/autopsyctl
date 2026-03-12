# end-session

Purpose
-------
Record the current development state before ending an AI or human work session.

This skill ensures the next session can resume quickly by updating SESSION.md
with accurate repository state and next actions.

Use When
--------
- Finishing a coding session
- Switching to another issue
- Preparing to commit and stop work
- Handing work to another agent
- Ending a Codex or ChatGPT interaction

Inputs
------
Repository working tree
Git branch state
GitHub issue context
SESSION.md (if present)

Procedure
---------

1. Inspect repository state

   Commands to simulate or suggest:

   git status
   git branch --show-current
   git log --oneline -n 3

2. Identify active issue

   Determine from:
   - branch name
   - recent commits
   - session discussion

3. Determine workflow stage

   Classify current work as one of:

   issue
   spec
   design
   implementation
   test
   debug
   review
   docs

4. Detect uncommitted work

   If working tree is dirty:

   - list modified files
   - note if commit or patch review is pending

5. Determine next task

   Choose the smallest forward step using workflow logic:

   spec missing -> issue-to-spec
   design missing -> design-review
   implementation incomplete -> implementation-plan
   tests missing -> test-generator
   patch pending -> patch-review
   docs outdated -> docs-sync

6. Update SESSION.md

   Write or update fields:

   Current branch:
   Current issue:
   Current stage:
   Current goal:
   Next step:
   Blockers:
   Commands to rerun:
   Notes:

7. Ensure clarity

   SESSION.md should be short and factual.
   Avoid design explanations or long commentary.

Output Format
-------------

Session Summary
---------------

Branch
Active Issue
Current Stage

Changes
-------

Modified files or pending commits.

Next Step
---------

Recommended next action for the following session.

SESSION.md Update
-----------------

Provide the exact content written to SESSION.md.