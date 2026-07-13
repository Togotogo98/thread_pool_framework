#include "../Include/ThreadPool.h"
#include <iostream>
#include <mutex>

/* Mutex for synchronizing print statements */
std::mutex print_mutex;

/* Add task - function */
void Sum(int firstNumber, int secondNumber)
{
    std::lock_guard<std::mutex> lock(print_mutex);
    std::cout << firstNumber << " + " << secondNumber << " = "
            << (firstNumber + secondNumber)
            << " (thread " << std::this_thread::get_id() << ")" << std::endl;
}

/* Multiply task - class */
class Multiplier
{
public:
    void operator()(int firstNumber, int secondNumber) const
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << firstNumber << " * " << secondNumber << " = "
                << (firstNumber * secondNumber)
                << " (thread " << std::this_thread::get_id() << ")" << std::endl;
    }
};


int main()
{
    std::cout << "---Thread Pool---" << std::endl;

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

    for (int i = 0; i < 4; i++)
    {
        pool.SubmitTask(Sum, i, i + 1);
    }

    Multiplier multiplier;
    for (int i = 0; i < 4; i++)
    {
        pool.SubmitTask(multiplier, i, i + 2);
    }

    return 0;
}