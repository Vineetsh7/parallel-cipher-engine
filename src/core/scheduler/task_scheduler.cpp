#include "task_scheduler.hpp"
#include "../cipher/cipher_engine.hpp"

// init pool with hw threads
ProcessManagement::ProcessManagement(int cipherKey) : key(cipherKey), stop(false) {
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) {
        threadCount = 8;
    }

    // start workers
    for (unsigned int i = 0; i < threadCount; ++i) {
        workers.emplace_back(&ProcessManagement::executeTask, this);
    }
}

ProcessManagement::~ProcessManagement() {
    // force cleanup
     waitForAll();
}

bool ProcessManagement::submitToQueue(Task task) {
    {
        std::unique_lock<std::mutex> lock(queueLock);

        // check limits
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
        Task currentTask("", Action::ENCRYPT); // reset

        {
            std::unique_lock<std::mutex> lock(queueLock);
            // wait for work
            cv.wait(lock, [this] { return stop || !taskQueue.empty(); });

            if (stop && taskQueue.empty()) {
                return; // exit thread
            }

            currentTask = std::move(taskQueue.front());
            taskQueue.pop();
        }

        // process task
        executeCryption(currentTask, key);
    }
}

void ProcessManagement::waitForAll() {
    {
        std::unique_lock<std::mutex> lock(queueLock);
        stop = true;
    }
    // wake workers
    cv.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}
