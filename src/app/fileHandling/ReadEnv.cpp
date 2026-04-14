#include "ReadEnv.hpp"
#include "IO.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string ReadEnv::readKey() {
    std::string env_path = ".env";
    IO io(env_path);
    std::fstream f_stream = io.getFileStream();

    if (!f_stream.is_open()) {
        throw std::runtime_error("Failed to open .env file — make sure it exists in the working directory");
    }

    std::stringstream buffer;
    buffer << f_stream.rdbuf();
    std::string content = buffer.str();

    // Trim leading and trailing whitespace / newlines so stoi() doesn't throw
    const std::string whitespace = " \t\n\r\f\v";
    content.erase(content.find_last_not_of(whitespace) + 1);
    content.erase(0, content.find_first_not_of(whitespace));

    return content;
}