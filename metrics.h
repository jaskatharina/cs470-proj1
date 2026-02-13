#include <iostream>
#include <chrono>
class Metrics {
    double exMinTime = 0;
    double decKeyTime = 0;
    double runTime = 0;
    int numHeapOps = 0;

    //used for start of overall runtime
    std::chrono::high_resolution_clock::time_point runtime_start;

    //used for storing starting points as needed when gathering metrics
    std::chrono::high_resolution_clock::time_point start;

    void RuntimeStart() {
        this->runtime_start = std::chrono::high_resolution_clock::now();
    }

    void RuntimeEnd() {
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        this->runTime = (end - runtime_start).count();
    }

    void StoreStartTime() {
        this->start = std::chrono::high_resolution_clock::now();
    }

    double GetDuration(){
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        return (end - start).count();
    }

    void Clear() {
        this->exMinTime = 0;
        this->decKeyTime = 0;
        this->runTime = 0;
        this->numHeapOps = 0;
    }

    void Print() {
        std::cout << "Total runtime: " << runTime << " s" << std::endl;
        std::cout << "Total time spent in decreaseKey: " << runTime << " s" << std::endl;
        std::cout << "Total time spent in extractMin: " << runTime << " s" << std::endl;
        std::cout << "Total number of heap operations: " << runTime << " s" << std::endl;
    }
};