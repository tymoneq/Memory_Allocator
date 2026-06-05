# Memory Allocator

A small custom memory allocator implemented in C. This project demonstrates a heap allocator built on top of `sbrk`, using a first-fit free block search strategy and a doubly linked list of block metadata.

## Project Structure

- `my_malloc/`
  - `allocator.c` - allocator implementation using `sbrk`, block headers, and page allocation.
  - `allocator.h` - public API and shared allocator data definitions.
  - `main.c` - example program that allocates, frees, and reuses memory.
  - `Makefile` - build rules for compiling the allocator and example program.
  - `DOCUMENTATION.md` - function-level documentation for the allocator internals.
- `area_allocator/`
  - `main.c` - placeholder folder for a separate allocator variant or future work.

## Overview

The allocator in `my_malloc/` manages dynamic memory by dividing heap space into blocks with metadata headers. Each block tracks its own size and usage state.

Key behaviors:
- Uses `sbrk` to request memory pages from the operating system.
- Allocates memory in 4096-byte pages (`PAGE_SIZE`).
- Uses a first-fit search through the block list to reuse free blocks.
- Tracks allocation statistics using an in-heap `block_counter` structure.
- Aligns allocations to 8-byte boundaries.

## Public API

The allocator exposes the following functions through `allocator.h`:

- `void *my_malloc(size_t size)`
  - Allocate `size` bytes from the custom heap.
  - Returns a pointer to usable memory or `NULL` on failure.

- `void my_free(void *ptr)`
  - Mark a previously allocated block as free.
  - The pointer must be one returned by `my_malloc`.

- `void free_all_pages()`
  - Return all allocated memory pages to the OS and reset allocator state.

## Build and Run

From the `my_malloc/` directory, run:

```sh
make
```

Then execute the example program:

```sh
./main
```

## Example Usage

The sample `main.c` performs a simple allocation cycle:

1. Allocate a block with `my_malloc`.
2. Free it with `my_free`.
3. Allocate again and reuse memory.
4. Call `free_all_pages()` to release memory back to the OS.

## Design Notes

The allocator stores metadata for each block in a `memory_block` header immediately before the payload. The header includes:

- `size` — block size plus a low-order bit used to indicate whether the block is in use.
- `next_block` / `prev_block` — pointers for the doubly linked list.

A separate `block_counter` structure is placed at the front of the heap to track:

- `MAGICAL_BYTES` marker
- number of pages allocated
- number of blocks created

## Limitations

Current implementation limitations include:

- `my_free` marks blocks as free, but coalescing of adjacent free blocks is not fully implemented.
- Blocks are not split when a larger free block is reused for a smaller request.
- No thread safety or concurrency support.
- The allocator assumes the pointer passed to `my_free` was returned by `my_malloc`.

## Notes

- The allocator currently relies on Linux-style `sbrk` behavior.
- The code compiles with AddressSanitizer and LeakSanitizer enabled via the provided `Makefile`.
- The repository is intended as an educational demonstration of low-level heap allocator mechanics.
# Memory Allocator

This repository contains a simple custom memory allocator implementation in C.
The main allocator lives in `my_malloc/` and demonstrates dynamic memory management using `sbrk`, a First-Fit free-block search, and a doubly linked list of allocation blocks.

## Project Structure

- `my_malloc/`
  - `allocator.c` — custom allocator implementation
  - `allocator.h` — public allocator API and shared types
  - `main.c` — example usage and basic test program
  - `Makefile` — build instructions for the allocator example
  - `DOCUMENTATION.md` — detailed function documentation for the allocator
- `area_allocator/`
  - `main.c` — placeholder file currently empty

## Overview

The allocator supports:

- `my_malloc(size_t size)` — allocate aligned memory from a heap managed with `sbrk`
- `my_free(void *ptr)` — mark a previously allocated block as free
- `free_all_pages()` — release all pages back to the OS and reset allocator state

It uses a first-fit strategy to search for reusable free blocks before requesting more memory.

## Build and Run

To build the allocator example:

```bash
cd my_malloc
make
```

Then run the binary:

```bash
./main
```

The provided `Makefile` compiles `main.c` and `allocator.c` with AddressSanitizer and LeakSanitizer enabled.

## Public API

Defined in `my_malloc/allocator.h`:

- `void *my_malloc(size_t size)`
  - Allocates a block of memory at least `size` bytes long.
  - Returns a pointer to the usable payload.

- `void my_free(void *ptr)`
  - Marks the block associated with `ptr` as free.
  - The allocator may reuse the block for subsequent allocations.

- `void free_all_pages()`
  - Releases all memory pages obtained from the OS.
  - Resets internal allocator state.

## Design and Implementation

### Heap Layout

The allocator reserves pages from the OS using `sbrk`.
The first page stores a small `block_counter` header at the beginning of the heap, which tracks:

- `number_of_pages`
- `number_of_blocks`

### Block Metadata

Each allocation block is represented by a `memory_block` header stored immediately before the user payload. The header contains:

- `size` — block size plus metadata bits for allocation state
- `next_block` — pointer to the next block in the list
- `prev_block` — pointer to the previous block in the list

Block sizes are aligned to 8 bytes.

### Allocation Strategy

When `my_malloc` is called:

1. The allocator initializes the heap on first use.
2. The requested size is rounded up to an 8-byte boundary.
3. The allocator scans the block list for the first free block large enough to satisfy the request.
4. If none is found, the allocator requests additional pages from the OS and creates a new block.

### Freeing

Calling `my_free(ptr)` marks the block as free.
A placeholder `coalesce` function exists for merging adjacent free blocks, but it is not implemented in the current version.

### Page Management

If an allocation cannot fit in the remaining heap space, the allocator computes how many additional 4096-byte pages are needed and requests them with `sbrk`.

## Limitations

- No block splitting is implemented.
- Coalescing free blocks is stubbed out and currently has no effect.
- Not thread-safe.
- The allocator assumes a single contiguous heap managed entirely by `sbrk`.

## Notes

For more detailed function-level documentation, see `my_malloc/DOCUMENTATION.md`.

This repository is a learning implementation of a custom allocator and is not intended as a production-grade memory manager.
