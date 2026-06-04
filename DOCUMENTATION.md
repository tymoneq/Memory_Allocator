# Memory Allocator Documentation

This document outlines the functions implemented in `allocator.c`. The allocator manages dynamic memory using a First-Fit search strategy, a doubly linked list for block tracking, and `sbrk` for requesting pages from the operating system.

---

## 1. Public API

These functions are intended to be exposed via `allocator.h` and used by the main application to allocate, manage, and free memory.

| Function | Signature | Description |
| :--- | :--- | :--- |
| **`my_malloc`** | `void* my_malloc(size_t size)` | The primary allocation entry point. Initializes the heap if necessary, searches for a free block of sufficient size, or requests new memory pages to create a new block. Returns a pointer to the usable memory payload. |
| **`set_block_free`** | `void set_block_free(memory_block* block)` | Marks a specific memory block as free (`used = false`), allowing it to be recycled by future `my_malloc` calls. |
| **`set_block_used`** | `void set_block_used(memory_block* block)` | Manually marks a specific memory block as in-use (`used = true`). |
| **`free_all_pages`** | `void free_all_pages()` | A complete cleanup function. Calculates the total memory used and returns it entirely to the OS via `sbrk(-size)`. Resets all global tracking pointers to `NULL`. |
| **`calculating_offset`** | `void* calculating_offset(memory_block* block_p)` | Utility function that takes a block metadata header and returns the pointer to the actual usable memory payload immediately following it. |

---

## 2. Internal Helpers (Static Functions)

These functions are marked `static` and are strictly for internal use within `allocator.c`. They handle the low-level interactions with the operating system and manage the linked list data structure.

### System Memory Management
* **`allocate(size_t size)`**
    * Wrapper around the `sbrk` system call. Requests raw memory from the OS and includes basic error checking.
* **`get_heap_end()`**
    * Uses `sbrk(0)` to retrieve the current memory address of the program break (the top of the heap).
* **`pages_to_allocate(uint64_t block_size, uint64_t heap_size)`**
    * Calculates exactly how many `PAGE_SIZE` (4096 bytes) chunks need to be requested from the OS to satisfy an allocation that exceeds the currently available heap space.
* **`allocate_new_pages(size_t size, uint64_t current_heap_size_free)`**
    * Executes the calculation from `pages_to_allocate` and requests the required number of pages via the `allocate()` function, updating the global statistics counter.

### Allocator State & Structure
* **`set_block_counter()`**
    * Initializes the allocator on its first run. Allocates the first page, sets up the "magic bytes" verification, and initializes the global `block_counter` (statistics tracking) at the very front of the heap.
* **`create_memory_block(size_t size)`**
    * Formats raw memory at the `block_end` boundary into a `memory_block` header. Sets the block's size and status to "used", and increments `block_end`.
* **`insert_new_block(memory_block* new_block)`**
    * Takes a newly formatted memory block and links it into the global doubly linked list, updating `first_block`, `last_block`, and total block statistics.
* **`create_new_memory_block(size_t size)`**
    * High-level internal function that orchestrates creating a new block: checks available heap space, triggers page allocation if out of room, creates the header, and inserts it into the linked list.

### Search Strategy
* **`find_free_node(size_t size)`**
    * Implements the **First-Fit** algorithm. Iterates sequentially through the linked list starting from `first_block` to find the first block that is both marked as free and large enough to satisfy the requested size. Returns `NULL` if no suitable block exists.