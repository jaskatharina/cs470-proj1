#ifndef METRICS_H
#define METRICS_H

#include <chrono>
#include <iostream>

// Simple timing / operation counter helper used by Prim/Dijkstra experiments.
// - Measures total runtime (seconds)
// - Accumulates time spent in extractMin and decreaseKey (seconds)
// - Counts heap-related operations (approx)


class Metrics {
public:
    Metrics() : exMinTimeSec(0.0), decKeyTimeSec(0.0), runTimeSec(0.0), numHeapOps(0) {}

    // overall runtime
    void startRun() { runtime_start = Clock::now(); }
    void stopRun()  { runTimeSec = std::chrono::duration<double>(Clock::now() - runtime_start).count(); }

    // extractMin timing
    void startExtractMin() { op_start = Clock::now(); }
    void stopExtractMin()  { exMinTimeSec += std::chrono::duration<double>(Clock::now() - op_start).count(); ++numHeapOps; }

    // decreaseKey timing
    void startDecreaseKey() { op_start = Clock::now(); }
    void stopDecreaseKey()  { decKeyTimeSec += std::chrono::duration<double>(Clock::now() - op_start).count(); ++numHeapOps; }

    // increment heap-op counter directly
    void addHeapOps(int n = 1) { numHeapOps += n; }

    // reset collected metrics
    void clear() { exMinTimeSec = decKeyTimeSec = runTimeSec = 0.0; numHeapOps = 0; }

    // print results
    void print(std::ostream &os = std::cout) const {
        os << "Total runtime: " << runTimeSec << " s\n"
           << "Time in decreaseKey: " << decKeyTimeSec << " s\n"
           << "Time in extractMin: " << exMinTimeSec << " s\n"
           << "Heap operations (approx): " << numHeapOps << std::endl;
    }

    // getters
    double totalRunTime() const { return runTimeSec; }
    double decreaseKeyTime() const { return decKeyTimeSec; }
    double extractMinTime() const { return exMinTimeSec; }
    int heapOps() const { return numHeapOps; }

private:
    using Clock = std::chrono::steady_clock;
    double exMinTimeSec;
    double decKeyTimeSec;
    double runTimeSec;
    int    numHeapOps;

    Clock::time_point runtime_start;
    Clock::time_point op_start; // reused for per-operation timing (single-threaded)
};

#endif // METRICS_H