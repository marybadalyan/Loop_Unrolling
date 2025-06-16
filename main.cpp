
#include <chrono>
#include <iostream>
#include "compiler_config.h"
#include <fstream>
#include <string>

namespace Color {
    const char* RESET = "\033[0m";
    const char* BOLD_CYAN = "\033[1;36m";
    const char* BOLD_YELLOW = "\033[1;33m";
    const char* BOLD_GREEN = "\033[1;32m";
    const char* BOLD_MAGENTA = "\033[1;35m";
}


int64_t sum_array(int32_t* a, int size) {
    int64_t sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += a[i];
    }
    return sum;
}

DISABLE_OPTIMIZATION

int64_t sum_array_unrolled(int32_t* a, int size) {
    int64_t sum = 0;
    for (int i = 0; i < size; i+=4) {
        sum += a[i];
        sum += a[i+1];
        sum += a[i+2];
        sum += a[i+3];
    }
    return sum;
}


RESTORE_OPTIMIZATION


void analyze_disassembly(const std::string& assemblyFilePath,const std::string& func_name) {
    std::ifstream asmFile(assemblyFilePath);
    if (!asmFile) {
        std::cerr << "Failed to open assembly file: " << assemblyFilePath << std::endl;
        return;
    }

    std::string line;
    bool inRunFunc = false;
    int count = 0;

    while (std::getline(asmFile, line)) {
        if (!inRunFunc) {
            if (line.find(func_name) != std::string::npos) {
                inRunFunc = true;
                std::cout << "\n--- Assembly for " << func_name << "() ---" << std::endl;
            }
        } else {
            if (line.empty() || line.find("ENDP") != std::string::npos || line.find(".cfi_endproc") != std::string::npos) {
                break;
            }
            if (!line.empty() && (line[0] == '\t' || line[0] == ' ')) {
                std::cout << line << std::endl;
                count++;
            }
        }
    }
    asmFile.close();

    std::cout << "--- End of Assembly ---\n" << std::endl;
    std::cout << Color::BOLD_GREEN << "Assembly Lines:     " << count <<  " instructions" << Color::RESET <<  std::endl;
}

int main(int argc, char* argv[]) {
    int32_t my_array[1024];
    for(int i = 0; i < 1024; ++i) {
        my_array[i] = i % 10;
    }

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <assembly_file_path>" << std::endl;
        return 1;
    }
    std::string assemblyFilePath = argv[1];
    

     
    auto start = std::chrono::high_resolution_clock::now();
    int64_t final_result = sum_array(my_array, 1024); // Store the result
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << Color::BOLD_CYAN << "Assembly Path:      " << assemblyFilePath << Color::RESET << std::endl;
    // std::cout << Color::BOLD_MAGENTA << "Optimization Level: " << optLevel <<   Color::RESET << std::endl;
    std::cout << Color::BOLD_YELLOW << "Time Taken:         " << elapsed.count() << " seconds" << Color::RESET <<  std::endl;
    // By printing the result, we guarantee to the compiler that the work was necessary.
    std::cout << Color::BOLD_YELLOW << "Final Result:       " << final_result << Color::RESET << std::endl;


    analyze_disassembly(assemblyFilePath,"sum_array");

    
     
    start = std::chrono::high_resolution_clock::now();
    final_result = sum_array_unrolled(my_array, 1024); // Store the result
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << Color::BOLD_CYAN << "Assembly Path:      " << assemblyFilePath << Color::RESET << std::endl;
    // std::cout << Color::BOLD_MAGENTA << "Optimization Level: " << optLevel <<   Color::RESET << std::endl;
    std::cout << Color::BOLD_YELLOW << "Time Taken:         " << elapsed.count() << " seconds" << Color::RESET <<  std::endl;
    // By printing the result, we guarantee to the compiler that the work was necessary.
    std::cout << Color::BOLD_YELLOW << "Final Result:       " << final_result << Color::RESET << std::endl;


    analyze_disassembly(assemblyFilePath,"sum_array_unrolled");

    return 0;

}




