#include <iostream>
#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp"
#include <thread>
#include <string>

// FIX #1: All POSIX IPC headers (sys/mman.h, semaphore.h, unistd.h,
// sys/fcntl.h, sys/wait.h) removed. Replaced by standard C++ threading.

ProcessManagement::ProcessManagement() {
    // Nothing to initialise — std::queue, std::mutex, and std::condition_variable
    // are default-constructed automatically.
}

ProcessManagement::~ProcessManagement() {
    // All resources are owned by RAII objects; nothing to clean up manually.
}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    std::string taskStr = task->toString();

    {
        std::unique_lock<std::mutex> lock(queueLock);

        // FIX #4: size check now lives here before the push (replaces the
        // unreachable dead check that was after sem_wait in the original code).
        if (static_cast<int>(taskQueue.size()) >= MAX_QUEUE_SIZE) {
            std::cerr << "Queue full — task rejected: " << taskStr << std::endl;
            return false;
        }

        taskQueue.push(taskStr);
    }  // lock released here — producers are never blocked by slow crypto

    cv.notify_one();  // wake exactly one waiting worker

    // Spawn one detached worker per task. The worker picks one item and exits.
    std::thread worker(&ProcessManagement::executeTask, this);
    worker.detach();

    return true;
}

void ProcessManagement::executeTask() {
    std::string taskStr;

    {
        std::unique_lock<std::mutex> lock(queueLock);
        // Wait until there is at least one item (handles spurious wakeups and
        // the case where notify_one fires before this thread acquires the lock).
        cv.wait(lock, [this] { return !taskQueue.empty(); });

        taskStr = taskQueue.front();
        taskQueue.pop();
    }  // FIX #3: lock released here — executeCryption runs without holding it,
       // so concurrent submitToQueue calls are never blocked by a slow task.

    executeCryption(taskStr);
}