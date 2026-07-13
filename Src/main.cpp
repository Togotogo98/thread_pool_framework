#include "../Include/ThreadPool.h"
#include <iostream>
#include <mutex>

/* Mutex for synchronizing print statements */
std::mutex print_mutex;


int main()
{
    std::cout << "Thread Pool Test" << std::endl;

    ThreadPool pool(4);

    /* Dummy tasks for testing */
    for (int i = 0; i < 8; i++)
    {
        pool.SubmitTask([i]
                        {
                            std::lock_guard<std::mutex> lock(print_mutex);
                            std::cout << "Task " << i << " running on thread "
                                    << std::this_thread::get_id() << std::endl;
                        });
    }
    return 0;
}