# Thread Pool Framework

A Thread Pool implementation that maintains a fixed number of worker threads and executes submitted tasks asynchronously.

---

## Features

- Fixed-size worker thread pool
- Thread-safe task queue
- Generic task submission
- Support for any callable (functions, lambdas, functors)
- Return values using `std::future`
- Graceful shutdown using RAII
- Performance benchmarking
- Thread utilization statistics

---

## Development Phases

### ✅ Phase 1 – Basic Thread Pool

- Fixed number of worker threads
- Shared task queue
- Worker thread execution loop

### ✅ Phase 2 – Generic Task Submission

- Generic `SubmitTask()` interface
- Support for arbitrary callables
- Template-based implementation

### ✅ Phase 3 – Futures & Packaged Tasks

- Asynchronous task execution
- Result retrieval using `std::future`
- `std::packaged_task` integration

### ✅ Phase 4 – Graceful Shutdown (RAII)

- Safe thread termination
- Condition variable notification
- Automatic cleanup in destructor
- Copy and move operations disabled

### ✅ Phase 5 – Performance Measurement

- Task execution latency
- Average latency
- Minimum latency
- Maximum latency
- 95th percentile latency
- Overall throughput

### ✅ Phase 6 – Per-thread task distribution and Stress Testing

- Thousands of submitted tasks
- Prime number computation benchmark - testing with comupation heavy task.
- Worker thread utilization analysis

---

## Building

Compile using g++:

```bash
g++ -std=c++17 -pthread Src/main.cpp Src/ThreadPool.cpp -I Include -o Build/thread_pool
```

Run:

```bash
./Build/thread_pool
```

---

## Sample Output

```
---Thread Pool---
------Benchmark Results:------
Number of Threads: 8
Number of Tasks: 100000
Task Distribution:
Thread ID: 124959632377536 - Tasks Completed: 12980
Thread ID: 124959657555648 - Tasks Completed: 9925
Thread ID: 124959873545920 - Tasks Completed: 13392
Thread ID: 124959615592128 - Tasks Completed: 11409
Thread ID: 124959623984832 - Tasks Completed: 12779
Thread ID: 124959640770240 - Tasks Completed: 12877
Thread ID: 124959649162944 - Tasks Completed: 13424
Thread ID: 124959607199424 - Tasks Completed: 13214

----- Thread Pool Statistics -----
Completed Tasks : 100000
Total Benchmark Time : 0.66455 seconds
Average Latency : 0.0311163 milliseconds
Minimum Latency : 0.014003 milliseconds
Maximum Latency : 4.1997 milliseconds
95th Percentile Latency : 0.035268 milliseconds
Throughput : 150478 tasks/second
```

## Author

**Rimjhim Chakraborty**
