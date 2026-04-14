#ifndef TASK_SCHEDULER_HPP
#define TASK_SCHEDULER_HPP

#include "work_item.hpp"
#include <memory>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>

class ProcessManagement {
public:
    // Initializes the thread pool and stores the global environment key
    ProcessManagement(int cipherKey);
    ~ProcessManagement();

    // Queues a task for the workers
    bool submitToQueue(Task task);

    // Waits for all tasks and cleanly shuts down the pool
    void waitForAll();

private:
    void executeTask();

    int key;
    std::vector<std::thread> workers;
    std::queue<Task> taskQueue;
    std::mutex queueLock;
    std::condition_variable cv;
    std::atomic<bool> stop;

    // Hard limit on queue size to prevent memory exhaustion on millions of files
    static constexpr size_t MAX_QUEUE_SIZE = 10000;
};

#endif
