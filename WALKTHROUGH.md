# GDB Learning Walkthrough

A step-by-step guide through each bug with GDB annotations explaining what each command shows and why it matters.

---

## Setup (Every Session)

Every time you start a new Docker container you need to install GDB:
```bash
docker run -it --rm -v $(pwd):/app gcc:latest bash
cd /app
apt-get update && apt-get install -y gdb valgrind
```

---

## Bug 01 — Use After Free

**What it is:** Reading memory that has already been freed back to the OS.
**Why it's dangerous:** The value may look valid sometimes and crash other times — unpredictable behavior.

```bash
cd /app/01_use_after_free
gcc -g -o buggy buggy.c
gdb ./buggy
```

```gdb
(gdb) break buggy.c:28
# Set breakpoint right before delete_list is called
# This pauses execution so we can inspect memory before and after free

(gdb) run
# Program starts and pauses at our breakpoint

(gdb) print *list
# Dereference list pointer — shows valid data: {data = 30, next = 0x...}
# Memory is still owned by our program here

(gdb) next
# Executes delete_list() — all nodes are now freed
# The list pointer still holds the old address but memory is released

(gdb) print *list
# Dereference the same pointer again — shows garbage or old value
# This is undefined behavior — we no longer own this memory

(gdb) next
# Executes the printf — reads freed memory
# May print garbage, may crash — depends on luck
```

**Confirm with ASan:**
```bash
gcc -g -fsanitize=address -o buggy buggy.c
./buggy
# ASan reports: heap-use-after-free at buggy.c:36
```

**Run the fix:**
```bash
gcc -g -o fixed fixed.c
./fixed
# Output: Data before deletion: 30
#         List successfully deleted.
# Clean exit — value read before free, pointer nulled after
```

**Key takeaway:** Always read values before freeing, and set pointers to NULL after freeing so any accidental access is caught immediately as a null dereference instead of silent corruption.

---

## Bug 02 — Buffer Overflow

**What it is:** Writing more data into a buffer than it can hold, corrupting adjacent memory.
**Why it's dangerous:** Can overwrite return addresses, enabling crashes or security exploits.

```bash
cd /app/02_buffer_overflow
gcc -g -o buggy buggy.c
gdb ./buggy
```

```gdb
(gdb) break copy_input
# Break at the start of copy_input function
# This lets us inspect the buffer before and after strcpy

(gdb) run
# Pauses at copy_input with input = "ThisStringIsWayTooLongForTheBuffer"

(gdb) list
# Shows the source — buffer[8] can only hold 8 characters
# strcpy will blindly write all 34 characters regardless

(gdb) print buffer
# Shows garbage/uninitialized stack memory before strcpy runs
# e.g. "\240W\263\246\377\377\000"

(gdb) next
# Executes strcpy — 34 bytes written into an 8 byte buffer
# 26 bytes overflow into adjacent stack memory

(gdb) print buffer
# Shows "ThisStri" — GDB only shows the 8 byte buffer
# But 26 more bytes were written past the end into the stack
```

**Confirm with ASan:**
```bash
gcc -g -fsanitize=address -o buggy buggy.c
./buggy
# ASan reports: stack-buffer-overflow at buggy.c:8
# "WRITE of size 35" into an 8 byte buffer
```

**Run the fix:**
```bash
gcc -g -o fixed fixed.c
./fixed
# Output: Buffer contains: ThisStr
# Safely truncated to 7 chars + null terminator
```

**Key takeaway:** Never use `strcpy` with untrusted input. Always use `strncpy` with `sizeof(buffer) - 1` and manually null terminate.

---

## Bug 03 — Null Pointer Dereference

**What it is:** Trying to read or write through a pointer that is NULL (address 0x0).
**Why it's dangerous:** Always causes an immediate SIGSEGV crash.

```bash
cd /app/03_null_dereference
gcc -g -o buggy buggy.c
gdb ./buggy
```

```gdb
(gdb) break main
# Break at start of main so we can step through

(gdb) run

(gdb) next
(gdb) next
(gdb) next
# Step through: list is NULL, find() searches an empty list, returns NULL

(gdb) print result
# Shows: $1 = (Node *) 0x0
# result is NULL — find() returned nothing
# The next line will try to read result->data from address 0x0

(gdb) next
# Program receives SIGSEGV — OS kills it for accessing address 0x0
# Output: Program received signal SIGSEGV, Segmentation fault.

(gdb) backtrace
# Shows the call stack at the moment of the crash
# Confirms crash happened at buggy.c:23 in main()
```

**Run the fix:**
```bash
gcc -g -o fixed fixed.c
./fixed
# Output: Value not found in list.
# NULL check prevents the dereference entirely
```

**Key takeaway:** Always check pointer return values for NULL before dereferencing. Any function that searches or allocates can return NULL on failure.

---

## Bug 04 — Memory Leak

**What it is:** Allocating heap memory and losing the pointer without freeing it.
**Why it's dangerous:** Silent — no crash. In long-running programs the process slowly consumes all available memory.

```bash
cd /app/04_memory_leak
gcc -g -o buggy buggy.c
gdb ./buggy
```

```gdb
(gdb) break main
(gdb) run

(gdb) next
# Enter the loop — list is declared as a local variable

(gdb) next
(gdb) print list
# Shows: $1 = (Node *) 0x0 — list starts as NULL

(gdb) next
# append() allocates a new node on the heap

(gdb) next
# printf prints the value

(gdb) next
# Loop iteration ends — list goes out of scope

(gdb) print list
# Shows: No symbol "list" in current context
# The pointer is gone — but the heap memory it pointed to still exists
# with no way to ever free it
```

**Confirm with Valgrind:**
```bash
valgrind --leak-check=full ./buggy
# Reports: definitely lost: 80 bytes in 5 blocks
# 5 nodes allocated, none freed — 16 bytes each = 80 bytes total
```

**Run the fix:**
```bash
gcc -g -o fixed fixed.c
valgrind --leak-check=full ./fixed
# Reports: All heap blocks were freed -- no leaks are possible
# 6 allocs, 6 frees — perfect balance
```

**Key takeaway:** Every `malloc` needs a matching `free`. Use Valgrind regularly to catch leaks that don't cause crashes but will kill long-running processes.

---

## Bug 05 — Double Free

**What it is:** Calling `free()` twice on the same pointer.
**Why it's dangerous:** Corrupts the heap allocator's internal structures, causing crashes or exploitable behavior.

```bash
cd /app/05_double_free
gcc -g -o buggy buggy.c
gdb ./buggy
```

```gdb
(gdb) break main
(gdb) run

(gdb) next
(gdb) next
(gdb) next
(gdb) print list
# Shows valid pointer e.g. $1 = (Node *) 0x2037b2a0
# Memory is allocated and valid

(gdb) next
# First delete_list call — memory at 0x2037b2a0 is freed
# Heap marks this block as available

(gdb) print list
# Still shows $2 = (Node *) 0x2037b2a0
# Pointer unchanged — still points to freed memory
# This is the dangerous moment

(gdb) next
# Second delete_list call — tries to free 0x2037b2a0 again
# Heap detects the corruption and calls abort()
# Program receives SIGABRT

(gdb) backtrace
# Shows the crash happened in the heap's free() internals
```

**Confirm with ASan:**
```bash
gcc -g -fsanitize=address -o buggy buggy.c
./buggy
# ASan reports: double-free at buggy.c:26
```

**Run the fix:**
```bash
gcc -g -o fixed fixed.c
./fixed
# Output: Node value: 99
#         List already freed — skipping second delete.
# Pointer nulled after first free — second call safely skipped
```

**Key takeaway:** After freeing a pointer always set it to NULL. A second `free(NULL)` is safe and does nothing. A second `free(ptr)` is heap corruption.

---

## Bug 06 — Stack Overflow

**What it is:** A function that recurses infinitely, filling the call stack until it overflows.
**Why it's dangerous:** Always crashes — no recovery possible once the stack is full.

```bash
cd /app/06_stack_overflow
gcc -g -o buggy buggy.c
gdb ./buggy
```

```gdb
(gdb) break countdown
# Break at every call to countdown
# We'll see it counting down past 0 into negative numbers

(gdb) run
# Pauses at first call: countdown(n=10)

(gdb) next
(gdb) next
# Already at n=-2 — it went past 0 with no base case to stop it

(gdb) backtrace
# Shows a huge wall of countdown() frames stacked on each other
# Each function call adds a frame — with no base case they pile up forever

(gdb) delete 1
# Remove the breakpoint so we can let it run freely

(gdb) continue
# Floods the terminal with negative numbers
# Eventually: Program received signal SIGSEGV
# Stack exhausted — even printf couldn't find stack space to run
```

**Run the fix:**
```bash
gcc -g -o fixed fixed.c
./fixed
# Output: Counting: 10
#         Counting: 9
#         ...
#         Counting: 0
#         Done!
# Base case n < 0 stops the recursion cleanly
```

**Key takeaway:** Every recursive function needs a base case that stops the recursion. Always trace through the smallest input to verify the base case is reachable.

---

## Summary

| # | Bug | Signal | Best Tool | Key Fix |
|---|-----|--------|-----------|---------|
| 01 | Use-after-free | Silent / ASan | GDB + ASan | Read before free, null after |
| 02 | Buffer overflow | SIGSEGV / ASan | GDB + ASan | Use strncpy, check sizes |
| 03 | Null dereference | SIGSEGV | GDB | Check for NULL before use |
| 04 | Memory leak | Silent | Valgrind | Match every malloc with free |
| 05 | Double free | SIGABRT | GDB + ASan | Null pointer after free |
| 06 | Stack overflow | SIGSEGV | GDB | Always have a base case |
