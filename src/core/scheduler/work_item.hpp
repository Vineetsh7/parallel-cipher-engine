// work_item.hpp
#ifndef WORK_ITEM_HPP
#define WORK_ITEM_HPP

#include <string>

enum class Action {
    ENCRYPT,
    DECRYPT
};

struct Task {
    std::string filePath;
    Action action;

    Task(const std::string& path, Action act)
        : filePath(path), action(act) {}
};

#endif
