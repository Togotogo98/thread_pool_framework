#include "../Include/ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
{
    if (num_threads == 0)
    {
        throw std::invalid_argument("ThreadPool must contain at least one worker thread.");
    }

    for(size_t i = 0; i < num_threads; i++)
    {
        workers.emplace_back(
            [this]()
            {
                while(true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock,
                            [this]
                            { 
                                return stop || !tasks.empty(); 
                            }                            
                        );
                    
                        if(stop && tasks.empty())
                            return;
                    
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            }
        );
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();

    /* joinable(): this returns true only if the thread object 
     * represents an active, unclaimed thread (meaning .join() or .detach() 
     * can still be legally called on it), and false otherwise.*/
    for(std::thread &worker: workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}