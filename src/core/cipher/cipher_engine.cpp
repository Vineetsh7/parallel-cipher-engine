#include "cipher_engine.hpp"
#include "../io/file_stream.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <ctime>
#include <iomanip>

int executeCryption(const Task& task, int key) {
    IO io(task.filePath);
    std::fstream f_stream = std::move(io.getFileStream());

    if (!f_stream.is_open()) {
        std::cerr << "Failed to open file for processing: " << task.filePath << std::endl;
        return 1;
    }

    // Process chunk by chunk (e.g. 64KB) to save memory and utilize CPU cache fully.
    constexpr size_t CHUNK_SIZE = 64 * 1024;
    std::vector<char> buffer(CHUNK_SIZE);

    // Keep going until EOF
    f_stream.seekg(0, std::ios::beg);
    while (f_stream) {
        std::streampos current_pos = f_stream.tellg();
        f_stream.read(buffer.data(), CHUNK_SIZE);
        std::streamsize bytes_read = f_stream.gcount();

        if (bytes_read == 0) {
            break;
        }

        // Apply cipher logic on the memory buffer
        for (std::streamsize i = 0; i < bytes_read; ++i) {
            if (task.action == Action::ENCRYPT) {
                buffer[i] = static_cast<char>((static_cast<unsigned char>(buffer[i]) + key) % 256);
            } else {
                buffer[i] = static_cast<char>((static_cast<unsigned char>(buffer[i]) - key + 256) % 256);
            }
        }

        // Write the processed chunk back to its original position
        f_stream.clear();  // Clear EOF bit if we hit the end of file during read
        f_stream.seekp(current_pos);
        f_stream.write(buffer.data(), bytes_read);
        
        // Seek 'g' pointer back to end of the written block to continue reading
        f_stream.seekg(current_pos + std::streamoff(bytes_read));
    }

    f_stream.close();

    // Standard output logging
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::cout << "Processed [" << (task.action == Action::ENCRYPT ? "ENCRYPT" : "DECRYPT") 
              << "] : " << task.filePath << " at "
              << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;

    return 0;
}
