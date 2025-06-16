cmake_minimum_required(VERSION 3.10)
project(Optimization_levels CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We only need ONE executable target. The configuration will change its properties.
add_executable(loop_unrolling main.cpp)

# Define the output directory for the analysis files
set(ANALYSIS_DIR "${CMAKE_BINARY_DIR}/analysis")
file(MAKE_DIRECTORY ${ANALYSIS_DIR})


# --- Let CMake handle the optimization flags ---
# We don't need to set /Od or /O2 manually. CMake does this for us
# based on the build type (Debug or Release).

# --- Correct Assembly Generation ---
if(MSVC)
    add_custom_command(
        TARGET loop_unrolling POST_BUILD
        # This command runs after the build for the given configuration
        COMMAND cmd /c "dumpbin /DISASM \"$<TARGET_OBJECTS:loop_unrolling>\" > \"${ANALYSIS_DIR}/assembly.txt\""
        COMMENT "Dumping assembly for $<CONFIG> build to analysis/assembly.txt"
    )
else() # For GCC/Clang
     add_custom_command(
        TARGET loop_unrolling POST_BUILD
        COMMAND objdump -d -M intel "$<TARGET_FILE:loop_unrolling>" > "${ANALYSIS_DIR}/assembly.txt"
        COMMENT "Dumping assembly for $<CONFIG> build to analysis/assembly.txt"
    )
endif()