// cipher_runner_alt.cpp
// Legacy/alternate entry point — renamed from cryption_main.cpp
#include <iostream>
#include "cipher_engine.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./cipher <task_data>" << std::endl;
        return 1;
    }
    executeCryption(argv[1]);
    return 0;
}
