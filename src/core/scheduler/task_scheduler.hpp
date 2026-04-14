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
    // constructor
    ProcessManagement(int cipherKey);
    ~ProcessManagement();

    // add task
    bool submitToQueue(Task task);

    // wait and shutdown
    void waitForAll();

private:
    void executeTask();

    int key;
    std::vector<std::thread> workers;
    std::queue<Task> taskQueue;
    std::mutex queueLock;
    std::condition_variable cv;
    std::atomic<bool> stop;

    // max queue size
    static constexpr size_t MAX_QUEUE_SIZE = 10000;
};

#endif
