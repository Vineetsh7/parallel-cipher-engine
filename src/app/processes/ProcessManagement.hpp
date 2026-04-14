#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

// FIX #1: Replaced POSIX-only IPC (shm_open, mmap, sem_open) with portable
// C++ standard library equivalents: std::queue + std::mutex + std::condition_variable.
// This compiles on Windows (native / WSL2), Linux, and macOS.

#include "Task.hpp"
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <iostream>

class ProcessManagement {
public:
    ProcessManagement();
    ~ProcessManagement();

    bool submitToQueue(std::unique_ptr<Task> task);
    void executeTask();

private:
    std::queue<std::string> taskQueue;  // in-process queue (replaces shared memory)
    std::mutex queueLock;
    std::condition_variable cv;

    static constexpr int MAX_QUEUE_SIZE = 1000;
};

#endif