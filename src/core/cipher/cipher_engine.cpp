#include "cipher_engine.hpp"
#include "../io/file_stream.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <cstdint>

int executeCryption(const Task& task, int key) {
    IO io(task.filePath);
    std::fstream f_stream = std::move(io.getFileStream());

    if (!f_stream.is_open()) {
        std::cerr << "Failed to open file for processing: " << task.filePath << std::endl;
        return 1;
    }

    // process in chunks
    constexpr size_t CHUNK_SIZE = 64 * 1024;
    std::vector<char> buffer(CHUNK_SIZE);

    // init prng state
    uint64_t stream_state = static_cast<uint64_t>(key) ^ 0x9E3779B97F4A7C15ULL;

    // read till eof
    f_stream.seekg(0, std::ios::beg);
    while (f_stream) {
        std::streampos current_pos = f_stream.tellg();
        f_stream.read(buffer.data(), CHUNK_SIZE);
        std::streamsize bytes_read = f_stream.gcount();

        if (bytes_read == 0) {
            break;
        }

        // apply cipher
        for (std::streamsize i = 0; i < bytes_read; ++i) {
            // prng step
            stream_state ^= stream_state >> 12;
            stream_state ^= stream_state << 25;
            stream_state ^= stream_state >> 27;
            uint64_t prng_output = stream_state * 0x2545F4914F6CDD1DULL;
            
            // get keystream byte
            unsigned char keystream_byte = static_cast<unsigned char>((prng_output >> 56) & 0xFF);
            
            // xor buffer
            buffer[i] ^= keystream_byte;
        }

        // write chunk back
        f_stream.clear();  // clear eof flag
        f_stream.seekp(current_pos);
        f_stream.write(buffer.data(), bytes_read);
        
        // reset read pointer
        f_stream.seekg(current_pos + std::streamoff(bytes_read));
    }

    f_stream.close();

    // log completion
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::cout << "Processed [" << (task.action == Action::ENCRYPT ? "ENCRYPT" : "DECRYPT") 
              << "] : " << task.filePath << " at "
              << std::put_time(now, "%Y-%m-%d %H:%M:%S") << std::endl;

    return 0;
}
