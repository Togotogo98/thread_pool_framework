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
#include <future>
#include <stdexcept>
#include <memory>


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
     * ReturnType : Whatever Callable(Arguments...) would return when 
     *              invoked
     * 
     * */
    template<typename Callable, typename... Arguments>
    auto SubmitTask(Callable&& callable, Arguments&&... arguments)
        -> std::future<std::invoke_result_t<Callable, Arguments...>>
    {
        /* std::result_of<Callable(Arguments...)>::type - not using the C++11 version.
         * C++17 version is std::invoke_result_t */
        using ReturnType = std::invoke_result_t<Callable, Arguments...>;
        /* 
         * std::bind packages the callable together with its arguments
         * into a single object "task", that takes ZERO arguments
         * when invoked later. This is required because the tasks queue
         * only knows how to store std::function<void()> objects. 
         * (note: function<void()> is a callable that takes no arguments and returns nothing
         * hence the need to bind the callable with its arguments into a single object that 
         * takes no arguments)
         *
         * std::forward preserves whether the original callable/arguments
         * were temporaries, avoiding unnecessary copies. 
         */
        auto task = std::bind(std::forward<Callable>(callable), 
                            std::forward<Arguments>(arguments)...);

        /* Create a packaged task and store it in a shared pointer.
         * The shared pointer is used to allow the task to be copied into the lambda
         * without affecting the original packaged task. Lambda is a copy of the packaged task, 
         * and when the lambda is executed, it will invoke the packaged task.
         * 
         * A packaged task is a wrapper for a callable (like a function or lambda) that
         * retrieves the result of the callable asynchronously using a future.
         * When packaged task is created, a future gets associated with it.
         * When the packaged task is executed, it runs the callable and sets the result
         * in the future, which can be retrieved later.
         */
        auto packagedTaskPtr = std::make_shared<std::packaged_task<ReturnType()>>(std::move(task));

        std::future<ReturnType> resultFuture = packagedTaskPtr->get_future();

        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            if (stop)
                throw std::runtime_error("SubmitTask called on a stopped ThreadPool");

            tasks.emplace([packagedTaskPtr]()
            {
                (*packagedTaskPtr)();
            });
        }
        condition.notify_one();
        return resultFuture;
    }
};
