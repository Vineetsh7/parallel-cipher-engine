#include <iostream>
#include <filesystem>
#include "./src/core/scheduler/task_scheduler.hpp"
#include "./src/core/io/env_config.hpp"
#include <ctime>
#include <iomanip>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    std::string directory;
    std::string action;

    std::cout << "Enter the directory path: ";
    std::getline(std::cin, directory);

    std::cout << "Enter the action (encrypt/decrypt): ";
    std::getline(std::cin, action);

    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            std::cerr << "Invalid directory path!" << std::endl;
            return 1;
        }

        // 1. Parse Config ONCE for all workers
        ReadEnv env;
        int cipherKey = 0;
        try {
            std::string keyStr = env.readKey();
            cipherKey = std::stoi(keyStr);
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid or missing encryption key in .env file." << std::endl;
            return 1;
        }

        // 2. Initialise the global Thread Pool
        ProcessManagement processManagement(cipherKey);
        Action taskAction = (action == "encrypt") ? Action::ENCRYPT : Action::DECRYPT;

        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        std::cout << "Starting directory traversal and processing at: " 
                  << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;

        // 3. Queue lightweight tasks seamlessly without maintaining giant OS blocks
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                Task task(filePath, taskAction);
                processManagement.submitToQueue(task);
            }
        }

        // 4. Clean exit after all workers finish
        processManagement.waitForAll();
        
        t = std::time(nullptr);
        now = std::localtime(&t);
        std::cout << "All processes completed successfully at: " 
                  << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;

    } catch (const fs::filesystem_error& ex) {
        std::cout << "Filesystem error: " << ex.what() << std::endl;
    }

    return 0;
}
