#include "Cryption.hpp"
#include "../processes/Task.hpp"
#include "../fileHandling/ReadEnv.hpp"   // FIX #2: include header, not .cpp
#include <ctime>
#include <iomanip>
#include <iostream>
#include <stdexcept>

int executeCryption(const std::string& taskData) {
    Task task = Task::fromString(taskData);

    // FIX #6: validate the encryption key before use
    ReadEnv env;
    std::string envKey = env.readKey();  // FIX #7: renamed from getenv()
    int key = 0;
    try {
        key = std::stoi(envKey);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Invalid encryption key in .env (\"" << envKey
                  << "\"). Must be a numeric integer." << std::endl;
        return 1;
    }

    // FIX #5: capture position with tellg() before get() then seekp(pos) before put()
    // This correctly overwrites the byte that was just read, regardless of
    // how the implementation manages the combined get/put position.
    char ch;
    while (true) {
        std::streampos pos = task.f_stream.tellg();
        if (!task.f_stream.get(ch)) break;

        ch = (task.action == Action::ENCRYPT)
                 ? static_cast<char>((static_cast<unsigned char>(ch) + key) % 256)
                 : static_cast<char>((static_cast<unsigned char>(ch) - key + 256) % 256);

        task.f_stream.seekp(pos);
        task.f_stream.put(ch);
        task.f_stream.flush();
    }
    task.f_stream.close();

    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::cout << "Completed encryption/decryption at: "
              << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;

    return 0;
}