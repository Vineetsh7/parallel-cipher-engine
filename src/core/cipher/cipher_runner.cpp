#include <iostream>
#include <string>
#include "cipher_engine.hpp"
#include "../io/env_config.hpp"

int main(int argc, char* argv[]) {
    // allow standalone use
    if (argc != 3) {
        std::cerr << "Usage: ./cipher <file_path> <ENCRYPT|DECRYPT>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::string actionStr = argv[2];
    Action action = (actionStr == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT;

    ReadEnv env;
    int cipherKey = 0;
    try {
        cipherKey = std::stoi(env.readKey());
    } catch (...) {
        std::cerr << "ERROR: Invalid encryption key in .env" << std::endl;
        return 1;
    }

    Task task(filePath, action);
    executeCryption(task, cipherKey);

    return 0;
}
