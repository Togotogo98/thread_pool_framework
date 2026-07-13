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
    
    /*
     * Callable : the type of the function being submitted
     *           (lambda, function pointer, functor, etc.)
     * Arguments : the types of whatever extra arguments that
     *             callable needs when it is eventually invoked
     * */
    template<typename Callable, typename... Arguments>
    void SubmitTask(Callable&& callable, Arguments&&... arguments)
    {
        /* 
         * std::bind packages the callable together with its arguments
         * into a single object, "boundTask", that takes ZERO arguments
         * when invoked later. This is required because the task queue
         * only knows how to store std::function<void()> objects. 
         * (note: function<void()> is a callable that takes no arguments and returns nothing)
         *
         * std::forward preserves whether the original callable/arguments
         * were temporaries, avoiding unnecessary copies. 
         */
        auto task = std::bind(std::forward<Callable>(callable), std::forward<Arguments>(arguments)...);
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(task);
        }
        condition.notify_one();
    }
};
