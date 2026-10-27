# Operating Systems Lab Assignment: xv6 Memory Management
**Student Name:** Manav Raj | **Roll Number:** 2401MC33 | **Institution:** IIT Patna

## Introduction
This lab explores core virtual and physical memory management mechanisms in the educational operating system xv6. Over the course of the assignment, we implemented custom system calls and kernel extensions to inspect virtual-to-physical address mappings, monitor physical frame utilization, print hierarchical page table structures, and optimize heap allocation using lazy demand paging.

---

## Question 1: Address Translation (va2pa)
### Design Decisions & Key Data Structures
- **Objective:** Translate a user virtual address (VA) to its corresponding physical address (PA) for the current process.
- **Mechanism:** Implemented a system call `va2pa` that accepts a virtual address from user space. The kernel utilizes xv6's page table walking utility (`walkpgdir`) on the process page directory (`myproc()->pgdir`) to locate the Page Table Entry (PTE). If present (`PTE_P`), the physical address is extracted using the `PTE_ADDR()` macro combined with the page offset.

---

## Question 2: Physical Memory Statistics (getmeminfo)
### Design Decisions & Key Data Structures
- **Objective:** Expose real-time kernel memory statistics to user-space programs via a custom `getmeminfo` system call.
- **Data Structures:** Defined `struct meminfo` in `types.h` to track `total_pages`, `free_pages`, `user_pages`, and `pt_pages`. Modified `kalloc.c` and hooked into `kmem` lock structures to efficiently track free physical pages (`free_pages`).
- **Memory Usage Statistics:**
  - **Initial State:** Total Pages: 57344, Free Pages: ~56788, User Pages: ~3-4, PT Pages: ~6-8.
  - **After malloc(8192):** Free pages decreased proportionally as the user heap expanded.
  - **After fork and Child Exit:** Free pages correctly returned to their pre-fork baseline after the child was reaped, confirming no memory leaks.

---

## Question 3: Page Table Dumping (vmprint)
### Design Decisions & Key Data Structures
- **Objective:** Print the hierarchical page table layout of a process for debugging and visualization.
- **Algorithm:** Iterates through the 1024 entries of the page directory (`pgdir`). For each valid Present PDE, it extracts the physical address, converts it to kernel virtual space using `P2V()`, and inspects the underlying page table entries, printing mappings and permission flags (`p`, `w`, `u`).

---

## Question 4: Lazy Memory Allocation (Demand Paging)
### Design Decisions & Key Data Structures
- **Objective:** Optimize memory utilization by deferring physical frame allocation during `sbrk()` until memory is actually accessed.
- **Design Changes:**
  - **`sys_sbrk`:** When growing the heap (`n > 0`), we simply increment `myproc()->sz += n` without allocating physical memory immediately.
  - **Page Fault Handler (`T_PGFLT`):** Caught page faults in `trap.c`, verified that the address lies within the valid heap range, allocated a physical frame via `kalloc()`, zero-filled it, and mapped it using `mappages()`.
  - **`copyuvm`:** Updated to gracefully skip unallocated page table entries rather than panicking, allowing lazy processes to fork safely.
- **Memory Usage Comparison:**
  - **Before sbrk(32MB):** Free Pages = 56788
  - **After sbrk(32MB) (Lazy):** Free Pages = 56788 (Zero physical memory consumed instantly).
  - **After touching 4 pages:** Free Pages = 56781 (Only accessed pages and page tables allocated on demand).
  - **After sbrk(-32MB) (Shrink):** Free pages successfully restored.

---

## Challenges Faced & Troubleshooting
1. **Compiler Strict Warnings (`-Werror`):** Newer GCC toolchains on Ubuntu treated minor indentation misalignments and implicit declarations as fatal errors. Resolved by wrapping multi-line `if` bodies in curly braces and adding proper `extern` function prototypes.
2. **Macro Case Sensitivity:** Corrected lowercase `p2v`/`v2p` macro names to xv6 standard uppercase `P2V`/`V2P`.
3. **Trap Handler Placement:** Integrated `T_PGFLT` cleanly as a case label inside the main `switch(tf->trapno)` block in `trap.c`.
