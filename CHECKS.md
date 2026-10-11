# Project Progress — Mini HTML Parser (domparser)

## Project Understanding

The goal is to build a basic HTML parser in **C** (and later, independently, in Python) that:

1. Reads an HTML file.
2. Identifies its structure: elements, attributes, text, and closing tags.
3. Builds a real **DOM (Document Object Model) tree** in memory — element nodes, text nodes, and parent/child relationships.
4. Traverses that tree to display or query it, e.g.:

```bash
domparser page.html
domparser page.html --tree
domparser page.html --text
domparser page.html --find-tag p
domparser page.html --find-id first
domparser page.html --find-class intro
```

### Target output style (tree view)

```text
.
└── html
    └── body
        ├── h1
        │   └── "Hello"
        ├── p [class="intro", id="first"]
        │   └── "Welcome to AI Karyashala"
        └── p [class="note"]
            └── "Learn by doing"
```

### Scope & Constraints

- **Supported subset:** opening/closing tags, nested and sibling elements, text content, attributes with double-quoted values, ignoring indentation-only whitespace.
- **Must detect errors:** unexpected closing tags, mismatched closing tags, unclosed elements.
- **Not allowed:** existing parsing libraries (`html.parser`, BeautifulSoup, `lxml`, `libxml2`, etc.) — parsing is hand-rolled.
- **Out of scope:** JavaScript, CSS, HTML entities, rendering, full HTML5 spec.
- Output must come from traversing the actual in-memory tree — not from echoing formatted input lines.
- C and Python implementations are independent but must produce identical output for equivalent inputs.
- Compiler: **clang**.

Development is **incremental, phase by phase**, guided step by step.

---

## Phase 1 — Read an HTML File and Print It ✅ COMPLETE

**Objective:** Prove end-to-end file I/O before any parsing: argument → open → read → print.

### Files created

| File | Purpose |
|---|---|
| `page.html` | Sample HTML input (from the assignment spec) |
| `domparser.c` | C program that reads an HTML file and prints its contents |

### What `domparser.c` implements

| Component | What it does |
|---|---|
| `argc` check | No file argument → prints `Usage: ./domparser <html-file>`, exits with failure |
| `fopen(argv[1], "r")` | Opens the HTML file for reading |
| `perror` on failure | Reports exact OS error (e.g., "No such file or directory") |
| `static char buffer[MAX_SIZE]` | 1 MB buffer (in BSS, not the stack) to hold the file |
| `fread` | Reads the whole file into memory in one call |
| `fclose` | Closes the file right after reading |
| Null-termination | `buffer[n] = '\0'` makes a valid C string |
| Empty-file check | `n == 0` → error message |
| `printf` + newline check | Prints contents; appends `\n` only if file doesn't end with one |
| `EXIT_SUCCESS` / `EXIT_FAILURE` | Standard exit codes (0 / 1) |

### Build & run

```bash
clang -Wall -Wextra -o domparser domparser.c
./domparser page.html
```

### Verified behavior

- [x] Compiles with `clang -Wall -Wextra` — **zero warnings**
- [x] `./domparser page.html` → prints full file with indentation preserved
- [x] `./domparser` (no args) → usage message, exit code 1
- [x] `./domparser nope.html` → "Error opening file: No such file or directory", exit code 1

### Deliberate Phase 1 simplifications (to revisit later)

- Fixed 1 MB buffer — later phases can switch to dynamic allocation.
- No parsing yet — tokenizer/DOM building comes in the next phases.

---

## Phase 2 — Tokenizer: Break the HTML into Pieces ✅ COMPLETE

**Objective:** Split the raw buffer into a stream of tokens — opening tags, closing tags, and text — and print each with its kind, one per line. Core insight: a parser first sees a stream of pieces, not a tree.

### Files created/changed

| File | Purpose |
|---|---|
| `nested.html` | Test file with nesting, siblings, and attributes |
| `domparser.c` | Added `tokenize()` on top of Phase 1 file reading |

### What the tokenizer implements

| Component | What it does |
|---|---|
| `TokenKind` enum | `TOK_OPEN_TAG`, `TOK_CLOSE_TAG`, `TOK_TEXT` |
| `tokenize(buf)` | Walks the buffer; `<` → scan to `>` (tag), else accumulate text until next `<` |
| Tag classification | `<` + `/` → `TOK_CLOSE_TAG`, otherwise `TOK_OPEN_TAG` |
| `is_all_whitespace` | Labels whitespace-only text as `TEXT(whitespace)` — the pieces later phases will ignore |
| `print_escaped` | Escapes `\n`, `\t`, `\r` so every byte is visible and accounted for |
| Whole-tag output | Attributes NOT split yet (Phase 3), e.g. `OPEN_TAG: <p class="intro" id="first">` |

### Build & run

```bash
clang -Wall -Wextra -o domparser domparser.c
./domparser nested.html
```

### Verified behavior

- [x] Compiles with `clang -Wall -Wextra` — **zero warnings**
- [x] `./domparser nested.html` → 19 tokens; concatenating all token contents reproduces the file byte-for-byte
- [x] Every whitespace-only text piece visible in escaped form (`"\n    "` etc.)
- [x] Adjacent tags (`</b></p>`) produce no spurious empty TEXT token
- [x] Phase 1 error handling intact: no-arg → usage, missing file → perror, both exit 1

---

## Upcoming Phases (planned, guided step by step)

- [ ] Phase 3 — Tag parsing: attributes, self-closing tags
- [ ] Phase 3 — Tag parsing: attributes, self-closing tags
- [ ] Phase 4 — DOM tree building + error detection (mismatched/unclosed tags)
- [ ] Phase 5 — Tree display (`tree`-style output), `--text`, search flags
