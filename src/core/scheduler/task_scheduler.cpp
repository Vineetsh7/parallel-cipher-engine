#include "task_scheduler.hpp"
#include "../cipher/cipher_engine.hpp"

// Use hardware thread capability, default to 8 if unable to detect
ProcessManagement::ProcessManagement(int cipherKey) : key(cipherKey), stop(false) {
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) {
        threadCount = 8;
    }

    // Spawn the thread pool
    for (unsigned int i = 0; i < threadCount; ++i) {
        workers.emplace_back(&ProcessManagement::executeTask, this);
    }
}

ProcessManagement::~ProcessManagement() {
    // If waitForAll() wasn't called manually, ensure graceful teardown
     waitForAll();
}

bool ProcessManagement::submitToQueue(Task task) {
    {
        std::unique_lock<std::mutex> lock(queueLock);

        // Optional safety ceiling to prevent out-of-memory on extreme counts
        if (taskQueue.size() >= MAX_QUEUE_SIZE) {
            std::cerr << "Warning: Queue full, rejecting task: " << task.filePath << std::endl;
            return false;
        }

        taskQueue.push(std::move(task));
    }
    cv.notify_one();
    return true;
}

void ProcessManagement::executeTask() {
    while (true) {
        Task currentTask("", Action::ENCRYPT); // Dummy init

        {
            std::unique_lock<std::mutex> lock(queueLock);
            // Wait for tasks or stop signal
            cv.wait(lock, [this] { return stop || !taskQueue.empty(); });

            if (stop && taskQueue.empty()) {
                return; // Graceful thread shutdown
            }

            currentTask = std::move(taskQueue.front());
            taskQueue.pop();
        }

        // Execute task out of lock to allow true parallelism
        executeCryption(currentTask, key);
    }
}

void ProcessManagement::waitForAll() {
    {
        std::unique_lock<std::mutex> lock(queueLock);
        stop = true;
    }
    // Wake up all threads so they can identify the stop signal and empty queues
    cv.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}
