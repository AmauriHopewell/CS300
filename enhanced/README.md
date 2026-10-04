# CS-300 ABCU course catalog (enhanced)

CS-499 Milestone Three. Original `Proj2.cpp` is in `../original/`. This folder is the same advising program with a binary search tree, a hash map, and a timed compare mode.

## Build (Visual Studio 2022)

From this `enhanced` folder, in x64 Native Tools or:

```
cmd /c "\"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat\" && cl /EHsc /std:c++17 /O2 Proj2.cpp /Fe:Proj2.exe"
```

Then:

```
Proj2.exe
```

Optional: `Proj2.exe path\to\file.csv`

## Menu

1. Load the CSV (default `ABCU_Advising_Program_Input.csv`)
2. Print all courses, in-order walk of the BST (sorted by course number)
3. Search one course through the hash map
4. Compare: same lookup on vector, BST, and hash, with elapsed time
5. Print the Big-O table
6. Print a topological schedule (a course never appears before its prerequisites)
9. Exit

## What stayed

Two-pass load: pass one records every course number; pass two keeps a row only if every prerequisite exists in that list. Text menu. `std::vector` as the CS-300 baseline.

## What changed

- CSV parser pushes the last cell (CSCI250 in the sample file has no trailing comma)
- `operator==` / `operator>` match their names
- Prerequisite lists are sorted by reference, so the sort is kept
- `file.is_open()` after open
- Menu choice is a local `int`, not `argc`
- Header comment: sort is by course number
- Hand-rolled BST and `std::unordered_map`, keyed by course number. The BST is filled median-first from the sorted vector so it does not collapse into a list.
- Rows with a missing prerequisite (see `BADPREREQ` in the sample) are dropped
- Menu 6: Kahn topological order, with a min-heap so ties are by course number

Compare mode runs 50,000 trials so the times are visible on a small catalog.
