//
// Created by scott on 21.05.2025.
//
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <functional>
#include <list>
#include <thread>

#ifndef EKSAMEN_WORKERS_H
#define EKSAMEN_WORKERS_H

using namespace std;

class Workers {
public:
    explicit Workers(int numThreads);
    void start();
    void post(function<void()> threadTask);
    void join();
private:
    int numThreads;
    mutex taskMutex;
    condition_variable cv;
    list<function<void()>> tasks ;
    vector<thread> worker_threads;
    atomic<bool> stopRunning = false;
};


#endif //EKSAMEN_WORKERS_H
