# GDB Learning Project

A hands-on collection of common C memory bugs to learn debugging with GDB, AddressSanitizer, and Valgrind.

## Setup

### Requirements
- Docker
- GDB
- GCC
- Valgrind (for memory leak detection)

### Running the Environment
```bash
docker run -it --rm -v $(pwd):/app gcc:latest bash
cd /app
apt-get update && apt-get install -y gdb valgrind
```

## Project Structure
```
gdbLearning/
├── 01_use_after_free/
│   ├── buggy.c       # Reads freed memory
│   └── fixed.c       # Reads before freeing, nulls pointer
├── 02_buffer_overflow/
│   ├── buggy.c       # strcpy overflows 8-byte buffer
│   └── fixed.c       # strncpy safely truncates input
├── 03_null_dereference/
│   ├── buggy.c       # Dereferences NULL return from find()
│   └── fixed.c       # Checks for NULL before dereferencing
├── 04_memory_leak/
│   ├── buggy.c       # Allocates nodes without freeing
│   └── fixed.c       # Frees each node before losing the pointer
├── 05_double_free/
│   ├── buggy.c       # Calls free() twice on same pointer
│   └── fixed.c       # Nulls pointer after free, checks before second free
└── 06_stack_overflow/
    ├── buggy.c       # Infinite recursion, no base case
    └── fixed.c       # Base case stops recursion at 0
```

## Bugs Overview

| # | Bug | Signal/Error | Tool |
|---|-----|-------------|------|
| 01 | Use-after-free | Garbage value / ASan report | GDB + ASan |
| 02 | Buffer overflow | Stack corruption / SIGSEGV | GDB + ASan |
| 03 | Null dereference | SIGSEGV | GDB |
| 04 | Memory leak | Silent / Valgrind report | Valgrind |
| 05 | Double free | SIGABRT | GDB + ASan |
| 06 | Stack overflow | SIGSEGV | GDB |

## Quick Reference: GDB Commands

| Command | Description |
|---------|-------------|
| `break file.c:N` | Set breakpoint at line N |
| `break function` | Set breakpoint at function entry |
| `run` | Start the program |
| `next` | Step over one line |
| `step` | Step into a function call |
| `continue` | Run until next breakpoint or crash |
| `print var` | Print value of variable |
| `print *ptr` | Dereference and print pointer |
| `backtrace` | Show the call stack |
| `list` | Show source around current line |
| `delete N` | Delete breakpoint N |
| `quit` | Exit GDB |

## Quick Reference: Compilation Flags

| Flag | Purpose |
|------|---------|
| `-g` | Include debug symbols |
| `-fsanitize=address` | Enable AddressSanitizer |
| `valgrind --leak-check=full` | Full memory leak report |
