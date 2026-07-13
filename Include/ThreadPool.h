/*****************************************************************************
 * This Header contains the definition of class ThreadPool.
 *
 * Description:
 * 1. workers: the pool’s worker threads.
 * 2. tasks: the shared queue of tasks waiting to run.
 * 3. queue_mutex: protects the task queue from concurrent access.
 * 4. condition: lets workers sleep until a task is available.
 * 5. stop: tells workers when the pool is shutting down.
 * 6. SubmitTask: the public function used to add work to the queue.
 *
*****************************************************************************/
#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>


class ThreadPool
{
private:

    std::vector<std::thread> workers;

    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;

    std::condition_variable condition;

    bool stop = false;

public:

    explicit ThreadPool(size_t num_threads);

    ~ThreadPool();

    void SubmitTask(std::function<void()> task);
};
