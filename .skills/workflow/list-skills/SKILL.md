# list-skills

Purpose
-------
Read `.skills/INDEX.md` and print the repository's available skills
in a clean, human-readable list.

This skill is intended as a lightweight skill discovery helper for
Codex sessions where built-in skill listing is not obvious enough.

Use When
--------
- you want to see available repo skills
- you forget the exact skill names
- you want a quick menu before starting work
- you want to confirm that `.skills/INDEX.md` is current

Inputs
------
.skills/INDEX.md

Procedure
---------
1. Read `.skills/INDEX.md`.
2. Parse category headings and the listed skills beneath them.
3. Print the skills grouped by category.
4. Preserve the names exactly as written in `.skills/INDEX.md`.
5. Do not invent skills that are not listed.
6. If `.skills/INDEX.md` is missing or incomplete, say so clearly.
7. Optionally highlight common starting points:
   - start-session
   - repo-state
   - next-task
   - end-session

Output Format
-------------

Available Skills
----------------

<category>
- <skill-name>
- <skill-name>

<category>
- <skill-name>
- <skill-name>

Suggested Starting Skills
-------------------------
- start-session
- repo-state
- next-task
- end-session

Rules
-----
- Treat `.skills/INDEX.md` as the source of truth.
- Do not scan the filesystem unless the index is missing.
- Keep output concise and readable.
- Preserve category structure from the index.