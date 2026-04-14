// scheduler_legacy.hpp
// Legacy/alternate scheduler header — renamed from process_management.hpp
#ifndef SCHEDULER_LEGACY_HPP
#define SCHEDULER_LEGACY_HPP

#include "work_item.hpp"
#include <queue>
#include <memory>

class ProcessManagement
{
public:
    ProcessManagement();
    bool submitToQueue(std::unique_ptr<Task> task);
    void executeTasks();

private:
    std::queue<std::unique_ptr<Task>> taskQueue;
};

#endif
