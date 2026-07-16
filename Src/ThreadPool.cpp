#include "../Include/ThreadPool.h"
#include <iostream>
#include <algorithm>
#include <numeric>

ThreadPool::ThreadPool(size_t numThreads)
{
    if (numThreads == 0)
    {
        throw std::invalid_argument("ThreadPool must contain at least one worker thread.");
    }

    for(size_t i = 0; i < numThreads; i++)
    {
        workers.emplace_back(
            [this]()
            {
                while(true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
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
                    auto taskStart = std::chrono::steady_clock::now();
                    task();
                    auto taskEnd = std::chrono::steady_clock::now();

                    double latency =
                            std::chrono::duration<double, std::milli>(taskEnd - taskStart).count();
                    {
                        std::lock_guard<std::mutex> lock(statsMutex);
                        taskLatencies.push_back(latency);
                        completedTasks++;
                    }
                }
            }
        );
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
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

void ThreadPool::StartBenchmark()
{
    benchmarkStart = std::chrono::steady_clock::now();
}

void ThreadPool::StopBenchmark()
{
    benchmarkEnd = std::chrono::steady_clock::now();
}

void ThreadPool::PrintStats() const
{
    if (taskLatencies.empty())
    {
        std::cout << "No benchmark data available." << std::endl;
        return;
    }

    double benchmarkTime =
        std::chrono::duration<double>(benchmarkEnd - benchmarkStart).count();

    double minLatency =
        *std::min_element(taskLatencies.begin(), taskLatencies.end());
    double maxLatency =
        *std::max_element(taskLatencies.begin(), taskLatencies.end());
    double totalLatency = 
        std::accumulate(taskLatencies.begin(), taskLatencies.end(), 0.0);
    
    double avgLatency = totalLatency / taskLatencies.size();

    std::cout << "\n----- Thread Pool Statistics -----\n";

    std::cout << "Completed Tasks : " << completedTasks << '\n';
    std::cout << "Total Benchmark Time : " << benchmarkTime << " seconds\n";
    std::cout << "Average Latency : " << avgLatency << " milliseconds\n";
    std::cout << "Minimum Latency : " << minLatency << " milliseconds\n";
    std::cout << "Maximum Latency : " << maxLatency << " milliseconds\n";
}