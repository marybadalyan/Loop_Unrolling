
# Loop Unrolling: Performance vs. Code Size Demo

This project demonstrates the concept of **loop unrolling**, a compiler optimization technique that can improve the performance of a program at the cost of increased binary size.

The program (`main.cpp`) times two functions: one with a standard loop and another with a manually unrolled loop. After execution, it analyzes the generated assembly code to show the difference in the number of instructions.



## What is Loop Unrolling?

Loop unrolling is an optimization technique where the body of a loop is duplicated one or more times to reduce the frequency of loop control instructions. In a typical loop, each iteration involves three key operations:

1.  **Work:** The actual operation being performed on the data.
2.  **Condition Check:** Verifying if the loop should continue (e.g., `i < size`).
3.  **Increment/Decrement:** Updating the loop counter (e.g., `++i`).

By "unrolling" the loop, you perform more work per iteration. For example, instead of processing one element at a time, you might process four. This means the condition check and increment instructions are executed only once for every four elements, significantly reducing the loop's overhead.

**The Trade-Off:**
* ✅ **More Efficient Time:** Fewer control instructions mean the CPU spends more of its time doing useful work, leading to faster execution.
* ❌ **Bigger Assembly:** Duplicating the work inside the loop body results in more lines of machine code, increasing the final size of the executable.



## How This Project Works

### C++ Code (`main.cpp`)

The core logic is in `main.cpp`. It performs the following steps:

1.  **Two Functions:**
    * `sum_array()`: A standard `for` loop that iterates through an array and sums its elements one by one.
    * `sum_array_unrolled()`: A loop that is manually unrolled to process **four** elements in each iteration.

2.  **Timing:** It uses `std::chrono` to measure the execution time of both functions, printing the results to the console.

3.  **Disassembly Analysis:** After running each function, the program calls `analyze_disassembly()`. This helper function opens the assembly file generated during the build process and counts the number of assembly instructions for `sum_array` and `sum_array_unrolled` to quantify the increase in code size.

### Controlling Compiler Optimizations (`compiler_config.h`)

To ensure a fair comparison and prevent the compiler from automatically optimizing our standard loop, we use specific macros to control its behavior.

* `DISABLE_OPTIMIZATION`: This macro inserts a compiler-specific pragma to turn off optimizations (`/Od` on MSVC, `O0` on GCC/Clang).
* `RESTORE_OPTIMIZATION`: This macro restores the default optimization settings.

In `main.cpp`, we wrap the `sum_array_unrolled` function between these macros. This allows the compiler to optimize the standard `sum_array` function (as it normally would in a Release build) while leaving our manually unrolled version untouched, giving us a clear view of the manual unrolling's impact.

```cpp
DISABLE_OPTIMIZATION
int64_t sum_array_unrolled(int32_t* a, int size) {
    // ... unrolled loop body ...
}
RESTORE_OPTIMIZATION
```

### Generating Assembly with CMake (`CMakeLists.txt`)

The project uses CMake to build the executable and automatically generate the disassembly file needed for analysis.

A `add_custom_command` is defined to run after the main executable (`loop_unrolling`) is built. This command inspects the compiled object and dumps its assembly representation into a text file.

* **On Windows (MSVC):** It uses `dumpbin /DISASM`.
* **On Linux/macOS (GCC/Clang):** It uses `objdump -d`.

This command saves the output to `build/analysis/assembly.txt`, which the C++ program then reads at runtime.

``` cmake
# For GCC/Clang
add_custom_command(
    TARGET loop_unrolling POST_BUILD
    COMMAND objdump -d -M intel "$<TARGET_FILE:loop_unrolling>" > "${ANALYSIS_DIR}/assembly.txt"
    COMMENT "Dumping assembly..."
)
```

### Expected Output

When you run the program, you will see output similar to this:

```
Optimization Level: O2
Assembly Path:      build//analysis//assembly.txt
Time Taken:         7e-07 seconds
Final Result:       4596

--- Assembly for sum_array() ---
  0000000000000000: 48 83 EC 08        sub         rsp,8
  0000000000000004: 45 33 C0           xor         r8d,r8d
  ......................................................
  000000000000006B: 66 0F 73 D8 08     psrldq      xmm0,8
  00000000000000B8: 48 8B 1C 24        mov         rbx,qword ptr [rsp]
  00000000000000BC: 49 3B C3           cmp         rax,r11
  00000000000000BF: 7D 07              jge         00000000000000C8
  00000000000000C1: 49 63 0C 82        movsxd      rcx,dword ptr [r10+rax*4]
  00000000000000C5: 4C 03 C9           add         r9,rcx
  00000000000000C8: 4A 8D 04 02        lea         rax,[rdx+r8]
  00000000000000CC: 49 03 C1           add         rax,r9
  00000000000000CF: 48 83 C4 08        add         rsp,8
  00000000000000D3: C3                 ret
  00000000000000D4: 49 8B C1           mov         rax,r9
  00000000000000D7: 48 83 C4 08        add         rsp,8
  00000000000000DB: C3                 ret
--- End of Assembly ---

Assembly Lines:     66 instructions
Assembly Path:      build//analysis//assembly.txt
Time Taken:         2.4e-06 seconds
Final Result:       4596

--- Assembly for sum_array_unrolled() ---
  0000000000000000: 89 54 24 10        mov         dword ptr [rsp+10h],edx
  0000000000000004: 48 89 4C 24 08     mov         qword ptr [rsp+8],rcx
  0000000000000009: 48 83 EC 18        sub         rsp,18h
 .......................................................
  00000000000000B9: 48 8B 44 24 08     mov         rax,qword ptr [rsp+8]
  00000000000000BE: 48 83 C4 18        add         rsp,18h
  00000000000000C2: C3                 ret
--- End of Assembly ---

Assembly Lines:     52 instructions
```

Notice that **`sum_array_unrolled` takes less time but has more assembly instructions**, clearly illustrating the performance-for-size trade-off of loop unrolling.
```