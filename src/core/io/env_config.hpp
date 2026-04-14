#ifndef ENV_CONFIG_HPP
#define ENV_CONFIG_HPP

#include <string>

class ReadEnv {
public:
    // Reads and returns the encryption key from the .env file.
    // Throws std::runtime_error if the file is unreadable or the key is invalid.
    std::string readKey();
};

#endif
