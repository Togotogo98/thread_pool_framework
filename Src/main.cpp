#include "../Include/ThreadPool.h"
#include <iostream>
#include <mutex>

/* Mutex for synchronizing print statements */
std::mutex printMutex;

/* Add task - function */
int Sum(int firstNumber, int secondNumber)
{
    return firstNumber + secondNumber;
}

/* Multiply task - class */
class Multiplier
{
public:
    int operator()(int firstNumber, int secondNumber) const
    {
        return firstNumber * secondNumber;
    }
};


int main()
{
    std::cout << "---Thread Pool---" << std::endl;

    ThreadPool pool(4);

    /* Vector to hold futures for the sum tasks */
    std::vector<std::future<int>> sumFutures;

    /* Submit tasks to the thread pool */
    for (int i = 0; i < 1000; i++)
    {
        std::future<int> sumResultFuture = pool.SubmitTask(Sum, i, i * 10);
        sumFutures.push_back(std::move(sumResultFuture));

        /*std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "Submitted task " << i 
                << " on thread " 
                << std::this_thread::get_id() 
                << std::endl;*/
    }

    /*for (size_t i = 0; i < sumFutures.size(); i++)
    {
        int result = sumFutures[i].get();
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "Result " << i << ": " << result << std::endl;
    }*/

    Multiplier multiplier;
    for (int i = 0; i < 4; i++)
    {
        std::future<int> multiplyResultFuture = pool.SubmitTask(multiplier, i, i + 1);
        int result = multiplyResultFuture.get();
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "Multiplication Result " << i << ": " << result << std::endl;
    }

    /* Combination of tasks Sum and Multiplier -  Square of sums */
    std::vector<std::future<int>> combinationFutures;
    for (int i = 0; i < 10; i++)
    {
        auto sumFuture = pool.SubmitTask(Sum, i, i + 1);
        int sumResult = sumFuture.get();//thread stuck? - main thread will have to wait here for 
                                        //the sum to be computed before it can submit the next task.
        auto SquareFuture = pool.SubmitTask(multiplier, sumResult, sumResult);
        combinationFutures.push_back(std::move(SquareFuture));
    }

    std::vector<std::future<int>> combinationFutures2;
    for (int i = 0; i < 10; i++)
    {
        auto sumFuture2 = pool.SubmitTask([i]()
                            {
                                int sum = Sum(i, i + 1);

                                Multiplier multiplier2;
                                return multiplier2(sum, sum);
                            });
        combinationFutures2.push_back(std::move(sumFuture2));
    }

    for (size_t i = 0; i < combinationFutures.size(); i++)
    {
        int result = combinationFutures[i].get();
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "Combination Result " << i << ": " << result << std::endl;
    }

    /* Square of sums but as a single task - version 2 
     * This is to avoid thread waiting to get the result */
    for (size_t i = 0; i < combinationFutures2.size(); i++)
    {
        int result = combinationFutures2[i].get();
        std::lock_guard<std::mutex> lock(printMutex);
        std::cout << "Combination Result 2 " << i << ": " << result << std::endl;
    }


    /* Benchmark */
    pool.PrintStats();

    return 0;
}