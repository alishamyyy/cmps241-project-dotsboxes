# plan.md: CMPS 241 Project, Dots and Boxes in C

**Team:** **Malek and Ali**
**Deadlines:** Sprint 1 is due **Sun Nov 15**. Sprint 2 is due **Sun Nov 29**. Sprint 3 (optional bonus) and the final submission are due **Thu Dec 3**. The presentation and demo happen in the last week of the semester.
**Today:** Sun Nov 1, so there are 14 days until Sprint 1.

---

## 1. Requirements (from the spec)

### 1.1 Grading
| Part | Weight |
|---|---|
| Report + presentation + GitHub repo + debugging strategy (GDB + Valgrind) | **50%** |
| Program correctness | **50%** |

- **A program that does not compile gets a zero.**
- All code must be on **GitHub**, with regular and meaningful commits from **both** students.
- Each team member must understand **every** part of the project, because either of you can be asked about anything in the demo.

### 1.2 Requirements for every sprint
- R1: The program is written in C and split into multiple `.h` and `.c` files.
- R2: A `Makefile` compiles the project.
- R3: The code is debugged with **Valgrind** (zero leaks and zero errors) and **GDB**, and both are documented in the report.
- R4: The game runs on **Alpine Linux** (no GUI) inside a **VM**.
- R5: **The game starts automatically when the VM boots**, with no username or password prompt.
- R6: Everything is on GitHub.

### 1.3 Sprint 1: Two-Player Game (due Nov 15)
- S1-1: Display a **4×5 grid of boxes**, which is **5 rows × 6 columns of dots** (rows 0–4, columns 0–5).
- S1-2: Players A and B take turns connecting two adjacent dots.
- S1-3: Input format is `r1 c1 r2 c2`, for example `0 4 0 3`.
- S1-4: Redraw the board after each move, along with `Player A score` and `Player B score`.
- S1-5: When a move completes a box, award it to that player and print their initial (`A` or `B`) inside it. **The same player then moves again** (this is what the sample run shows).
- S1-6: When all 20 boxes are claimed, declare the winner (most boxes) or a tie.
- S1-7: Reject invalid input: out-of-range dots, dots that aren't adjacent, diagonal lines, lines already drawn, and non-numeric input.

Board format, matching the sample:
```
  0 1 2 3 4 5
0 . . . .-. .
        |A| |
1 . . . .-. .
```
Horizontal line = `-`, vertical line = `|`, claimed box = owner's initial.

### 1.4 Sprint 2: Hard-Level Bot (due Nov 29)
- S2-1: A menu option for **Player vs Player** or **Player vs Bot**.
- S2-2: The bot only makes valid moves and never repeats one.
- S2-3: Show the updated board and both scores after every move. Declare the winner at the end.
- S2-4: The bot uses a **sophisticated strategy** of your own choosing (see §4).
- S2-5: Compute the **time complexity** of the strategy and of key operations such as the "is this box completed?" check. This goes in the report.
- S2-6: Submit the **final report** and a **PowerPoint**, then present and demo in the last week.

### 1.5 Sprint 3: Bonus, optional (due Dec 3)
- S3-1: **Network play.** Two players on two separate machines, using TCP sockets.
- S3-2: **Multithreading with locks** for one part of the project, for example a receiver thread plus a `pthread_mutex` on the shared board.

### 1.6 Final submission (Dec 3)
- The GitHub repo, up to date.
- **`groupname.DotsAndBoxes.zip`** (or `.rar`) containing all programs, uploaded to **Moodle**.
- The report (PDF) and slides (PPTX).

---

## 2. How: Architecture and repo layout

**Stack:** C (C99), `gcc`, `make`, `gdb`, `valgrind`, Alpine Linux in VirtualBox/UTM, Git and GitHub. Sprint 3 adds POSIX sockets and `pthread`.

```
DotsAndBoxes/
├── Makefile
├── README.md            # build/run instructions, team, VM setup
├── include/
│   ├── board.h          # Board struct + API
│   ├── game.h           # turn loop, scoring, winner
│   ├── input.h          # safe parsing/validation of moves
│   ├── display.h        # printing the board/scores
│   ├── bot.h            # Sprint 2
│   └── net.h            # Sprint 3
├── src/
│   ├── main.c           # menu: PvP / PvBot / Network
│   ├── board.c
│   ├── game.c
│   ├── input.c
│   ├── display.c
│   ├── bot.c            # Sprint 2
│   └── net.c            # Sprint 3
├── tests/
│   └── test_board.c     # unit tests for box detection, validity
├── vm/
│   └── autostart.md     # exact Alpine autologin/autostart steps + config files
└── docs/
    ├── report.md / report.pdf
    ├── valgrind_output.txt
    ├── gdb_session.txt
    └── slides.pptx
```

### 2.1 Data model
```c
#define ROWS 5            // dots
#define COLS 6
typedef struct {
    int h[ROWS][COLS-1];  // horizontal lines: h[r][c] = line (r,c)-(r,c+1)
    int v[ROWS-1][COLS];  // vertical lines:   v[r][c] = line (r,c)-(r+1,c)
    char box[ROWS-1][COLS-1]; // ' ', 'A', 'B'
    int score[2];
    int current;          // 0 = A, 1 = B
    int lines_left;       // 49 total lines
} Board;
```
- Total lines: 5×5 + 4×6 = 49. Total boxes: 20.
- `apply_move` returns the number of boxes it completed (0, 1, or 2). If that number is 0, the turn passes to the other player.
- Checking whether a box is complete is **O(1)** because it reads 4 sides. A line borders at most 2 boxes, so each move costs O(1). Scanning the whole board is O(R·C).
- Use plain arrays only, with no `malloc`. This makes Valgrind results clean. If you do use heap memory, `free` every allocation.

### 2.2 Makefile, in short
```make
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g -Iinclude
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
dots: $(OBJ) ; $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
clean: ; rm -f src/*.o dots
valgrind: dots ; valgrind --leak-check=full --show-leak-kinds=all ./dots
```
For Sprint 3, add `LDFLAGS=-pthread`.

### 2.3 VM autostart on Alpine (R5)
1. Install Alpine Linux ("virtual" ISO) in VirtualBox with `setup-alpine`. Then run `apk add build-base gdb valgrind git`.
2. Clone the repo and run `make`. Copy the binary to `/usr/local/bin/dots`.
3. Set up autologin on tty1 by editing `/etc/inittab`:
   `tty1::respawn:/bin/login -f root`
   (Alternatively, run the game directly: `tty1::respawn:/usr/local/bin/dots`.)
4. Make the game launch after autologin with `/root/.profile` containing `exec /usr/local/bin/dots`.
5. Reboot and check that the game appears with no login prompt. Record the steps and a screenshot in `vm/autostart.md` and in the report.

---

## 3. Who does what

| Area | Owner |
|---|---|
| Coding | **Malek and Ali, 50/50 by effort.** Rebalance modules and bug fixes as needed. |
| Sprint 1 coding | **Malek:** board, scoring and turn rules. **Ali:** input, display, game loop, menu and Makefile. |
| Sprint 2 coding | **Malek:** minimax and alpha-beta. **Ali:** capture and safe-move heuristics and chain detection. **Both:** endgame integration. |
| Optional bonus coding | **Malek:** TCP host and client. **Ali:** receiver thread and locks. |
| Testing, debugging and Alpine VM | **Malek:** run tests, GDB and Valgrind, verify autostart, and capture logs and screenshots. |
| PowerPoint and GitHub | **Ali:** create slides, organize the repo and README, and tag releases. Both make meaningful commits and review each other's code. |
| Report | **Malek:** write the testing, debugging and VM sections. **Ali:** assemble and format the PDF. Both write up their design and complexity analysis. |
| Submission | **Malek:** sign off on the packaged build and evidence before upload. **Ali:** package the ZIP and upload it with the PDF and slides after sign-off. **Both:** verify the Moodle submission receipt. |
| Presentation and demo | **Ali:** lead the slides. **Malek:** lead the live demo. Both rehearse and answer questions. |

---

## 4. Bot strategy for Sprint 2, recommended

Use a layered "hard" bot:
1. **Greedy capture:** if a move completes a box, take it.
2. **Safe move:** otherwise pick a line that doesn't create a box with 3 sides, so the opponent gets nothing for free.
3. **Endgame (no safe moves left):** split the board into **chains and loops**. Give away the **smallest** chain first. Apply the **double-cross / long-chain rule**: when taking a long chain (≥3), leave the last 2 boxes for the opponent so you keep control.
4. **Optional, for extra strength:** **minimax with alpha-beta pruning**, depth-limited, once ≤ ~12 lines remain. The evaluation function is `myScore − oppScore`.

**Complexity (for the report):**
- Box check: O(1). Generating moves: O(L), where L = 49 lines.
- Greedy and safe-move steps: O(L) per move.
- Chain analysis: O(B), where B = 20 boxes, using BFS/DFS over boxes that have 2 sides drawn.
- Minimax: O(b^d) in the worst case, and roughly O(b^(d/2)) with good move ordering for alpha-beta (b = remaining lines, d = depth).

---

## 5. Schedule

### Sprint 1: Nov 1 → Nov 15
| Date | Task | Who |
|---|---|---|
| Sun Nov 1 | Create the GitHub repo and folder layout, write the Makefile skeleton, read this plan together | Ali (repo, layout and Makefile), both (plan review) |
| Mon Nov 2 – Tue Nov 3 | `Board` struct, `init`, `apply_move`, box detection | Malek |
| Mon Nov 2 – Tue Nov 3 | `print_board` (sample format), score display | Ali |
| Wed Nov 4 – Thu Nov 5 | Input parsing and validation (adjacency, range, duplicates, junk input) | Ali |
| Wed Nov 4 – Thu Nov 5 | Turn loop, extra turn on capture, winner/tie | Ali |
| Fri Nov 6 – Sat Nov 7 | Alpine VM install, gcc/gdb/valgrind setup | Malek |
| Fri Nov 6 – Sat Nov 7 | Unit tests and edge cases (double box, last move) | Malek |
| Sun Nov 8 – Mon Nov 9 | **Autostart with no login** working in the VM | Malek |
| Sun Nov 8 – Mon Nov 9 | Valgrind run (0 leaks), GDB session saved to `docs/` | Malek |
| Tue Nov 10 – Wed Nov 11 | Full playthroughs in the VM, bug fixes | Malek (testing and debugging), both (fixes) |
| Thu Nov 12 | Code freeze, README, Sprint 1 notes for the report | Ali (README), both (code freeze and report notes) |
| Fri Nov 13 – Sat Nov 14 | Buffer: final checks with `make clean && make`, no warnings | Malek |
| **Sun Nov 15** | **Sprint 1 due.** Tag `v1.0-sprint1` and push | Ali |

### Sprint 2: Nov 16 → Nov 29
| Date | Task | Who |
|---|---|---|
| Mon Nov 16 | PvP/PvBot menu, `bot.h` API (`Move bot_choose(Board*)`) | Ali |
| Tue Nov 17 – Wed Nov 18 | Bot levels 1 and 2 (greedy and safe) | Ali |
| Tue Nov 17 – Wed Nov 18 | Chain/loop detection | Ali |
| Thu Nov 19 – Sat Nov 21 | Endgame strategy (give the smallest chain, double-cross) | Malek and Ali |
| Sun Nov 22 – Mon Nov 23 | Optional minimax with alpha-beta for the endgame | Malek |
| Sun Nov 22 – Mon Nov 23 | Test the bot: 0 invalid or repeated moves over 50+ games | Malek |
| Tue Nov 24 | Valgrind and GDB on the bot code, rebuild in the VM, check autostart | Malek |
| Wed Nov 25 – Thu Nov 26 | Complexity analysis written up, report draft | Both (analysis), Ali (PDF), Malek (evidence) |
| Fri Nov 27 – Sat Nov 28 | Slides draft, rehearse the demo, buffer | Ali (slides), both (rehearsal) |
| **Sun Nov 29** | **Sprint 2 due.** Tag `v2.0-sprint2` | Ali |

### Sprint 3 (bonus) and final: Nov 30 → Dec 3
| Date | Task | Who |
|---|---|---|
| Mon Nov 30 | TCP server/client (`net.c`): host waits, client connects, moves are sent as `r1 c1 r2 c2` | Malek |
| Mon Nov 30 | Receiver thread plus `pthread_mutex` around the board | Ali |
| Tue Dec 1 | Test on two machines/VMs (bridged network), Valgrind with threads | Malek (testing), Ali (second player) |
| Wed Dec 2 | Finish the report and slides, build the zip | Ali (PDF, slides and ZIP), Malek (verify build and evidence) |
| **Thu Dec 3** | **Final:** sign off on the packaged build and evidence, push and tag `v3.0-final`, upload `groupname.DotsAndBoxes.zip` + report + PPTX to **Moodle**, and verify the receipt | Malek (sign-off), Ali (push, tag and upload), both (receipt) |
| Last week | Presentation and live demo in the VM | Ali (slides), Malek (demo), both (Q&A) |

> Sprint 3 is a bonus. If time gets tight, drop it. The report, slides and a clean Sprint 2 matter more.

---

## 6. Report outline (part of the 50% component)
1. Introduction and rules
2. Design: module diagram, `Board` structure, line indexing
3. Sprint 1 implementation, plus how input validation works
4. Bot strategy: layers, chain theory, pseudocode
5. **Complexity analysis:** box check, move generation, each bot layer, minimax
6. **Debugging strategy:** Valgrind commands and output (0 leaks), GDB breakpoints/watch/backtrace examples, bugs found and how they were fixed
7. Alpine VM setup and autostart, with screenshots
8. Sprint 3 networking and threads (if done)
9. Testing, team contributions, conclusion

## 7. Definition of Done for each sprint
- [ ] `make clean && make` passes with **no errors** (and ideally no warnings)
- [ ] Valgrind shows `All heap blocks were freed` and `0 errors`
- [ ] The game auto-launches in the Alpine VM with no login prompt
- [ ] Output matches the sample format
- [ ] Code is pushed and the release is tagged on GitHub, with commits from both students
- [ ] Both students can explain every file
