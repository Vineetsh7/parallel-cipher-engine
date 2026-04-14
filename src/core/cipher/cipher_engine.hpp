#ifndef CIPHER_ENGINE_HPP
#define CIPHER_ENGINE_HPP

#include "../scheduler/work_item.hpp"
#include <string>

int executeCryption(const Task& task, int key);

#endif
