# implementation-plan

Purpose
-------
Turn a spec and design into an ordered, incremental coding plan.

Procedure
---------
1. Read the spec and design.
2. Break work into the smallest meaningful slices.
3. For each slice define files to change, interface impact, validation command, and likely commit boundary.
4. Prefer slices that compile and can be tested incrementally.
5. Call out risky steps needing extra tests or instrumentation.
