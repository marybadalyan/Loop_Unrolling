
#include <chrono>
#include <iostream>


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


int main(int argc, char* argv[]) {
    int32_t my_array[1024];
    for(int i = 0; i < 1024; ++i) {
        my_array[i] = i % 10;
    }

    return 0;

}

