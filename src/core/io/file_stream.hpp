// file_stream.hpp
#ifndef FILE_STREAM_HPP
#define FILE_STREAM_HPP

#include <fstream>
#include <string>

class IO {
public:
    IO(const std::string& file_path);
    ~IO();
    std::fstream getFileStream();

private:
    std::fstream file_stream;
};

#endif
