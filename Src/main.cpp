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

bool IsPrime(int number)
{
    if (number < 2)
        return false;

    for (int i = 2; i * i <= number; i++)
    {
        if (number % i == 0)
            return false;
    }

    return true;
}

int CountPrimes(int limit)
{
    int count = 0;

    for (int i = 2; i <= limit; i++)
    {
        if (IsPrime(i))
            count++;
    }

    return count;
}

void PrintBenchmarkResults( size_t numThreads, 
                            size_t numTasks, 
                            const ThreadPool& benchmarkPool)
{
    std::cout << "------Benchmark Results:------\n";
    std::cout << "Number of Threads: " << numThreads << "\n";
    std::cout << "Number of Tasks: " << numTasks << "\n";

    std::cout << "Task Distribution:\n";
    for (const auto& entry : benchmarkPool.workerTaskCounts)
    {
        std::cout << "Thread ID: " << entry.first 
                    << " - Tasks Completed: " << entry.second << "\n";
    }
}

void RunBenchmark(size_t numThreads, size_t numTasks)
{
    ThreadPool benchmarkPool(numThreads);
    std::vector<std::future<int>> benchmarkFutures;
    
    benchmarkPool.StartBenchmark();

    for (size_t i = 0; i < numTasks; i++)
    {
        benchmarkFutures.push_back(
            benchmarkPool.SubmitTask(CountPrimes, 1000));
    }

    for (auto& future : benchmarkFutures)
    {
        future.get();
    }

    benchmarkPool.StopBenchmark();

    PrintBenchmarkResults(numThreads, numTasks, benchmarkPool);
    benchmarkPool.PrintStats();
}

int main()
{
    std::cout << "---Thread Pool---" << std::endl;

    ThreadPool pool(4);

    /* Vector to hold futures for the sum tasks */
    std::vector<std::future<int>> sumFutures;

    //pool.StartBenchmark();
    /* Submit tasks to the thread pool */
    for (int i = 0; i < 10; i++)
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
    for (int i = 0; i < 40; i++)
    {
        std::future<int> multiplyResultFuture = pool.SubmitTask(multiplier, i, i + 1);
        int result = multiplyResultFuture.get();
        //std::lock_guard<std::mutex> lock(printMutex);
        //std::cout << "Multiplication Result " << i << ": " << result << std::endl;
    }

    /* Combination of tasks Sum and Multiplier -  Square of sums */
    std::vector<std::future<int>> combinationFutures;
    for (int i = 0; i < 100; i++)
    {
        auto sumFuture = pool.SubmitTask(Sum, i, i + 1);
        int sumResult = sumFuture.get();//thread stuck? - main thread will have to wait here for 
                                        //the sum to be computed before it can submit the next task.
        auto SquareFuture = pool.SubmitTask(multiplier, sumResult, sumResult);
        combinationFutures.push_back(std::move(SquareFuture));
    }

    std::vector<std::future<int>> combinationFutures2;
    for (int i = 0; i < 100; i++)
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
        //std::lock_guard<std::mutex> lock(printMutex);
        //std::cout << "Combination Result " << i << ": " << result << std::endl;
    }

    /* Square of sums but as a single task - version 2 
     * This is to avoid thread waiting to get the result */
    for (size_t i = 0; i < combinationFutures2.size(); i++)
    {
        int result = combinationFutures2[i].get();
        //std::lock_guard<std::mutex> lock(printMutex);
        //std::cout << "Combination Result 2 " << i << ": " << result << std::endl;
    }

    //pool.StopBenchmark();

    /* Benchmark */
    //pool.PrintStats();


    /* Benchmarking with different number of threads and tasks */
    RunBenchmark(8, 100000);

    return 0;
}