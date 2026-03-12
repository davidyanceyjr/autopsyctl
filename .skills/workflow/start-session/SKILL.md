# start-session

Purpose
-------
Initialize an AI-assisted development session and determine the
current repository state.

This skill provides a bootstrap routine that orients the agent
before any implementation advice is given.

Use When
--------
- Beginning a new AI or Codex session
- Returning to the repository after time away
- Repository state is unclear
- Multiple issues or branches exist

Inputs
------
Repository working tree
SESSION.md
GitHub issues
Repository structure

Procedure
---------

1. Read AGENTS.md

   Understand repository rules:
   - Phase-Oriented C style
   - air-gap constraints
   - dependency limitations
   - repository intent

2. Read SESSION.md (if present)

   Extract:
   - current branch
   - current issue
   - current workflow stage
   - next step notes
   - blockers

3. Inspect repository state

   Commands to simulate or suggest:

   git status
   git branch --show-current
   git log --oneline -n 5

4. Inspect repository structure

   Check presence of:

   spec/
   docs/design/
   src/
   tests/

5. Inspect GitHub issues

   gh issue list

6. Determine workflow position

   For the active issue determine:

   issue exists
   spec exists
   design exists
   implementation exists
   tests exist

7. Produce a state summary

   Output should include:

   Repository State
   Active Issue
   Workflow Stage
   Recent Changes
   Missing Artifacts

8. Recommend the next step

   Use workflow skills such as:

   issue-to-spec
   design-review
   implementation-plan
   test-generator
   patch-review
   docs-sync

Output Format
-------------

Repository State
----------------

Active Issue:
Current Branch:
Workflow Stage:

Findings
--------

What artifacts exist and what is missing.

Recommended Next Action
-----------------------

Skill to use next
Reason for recommendation