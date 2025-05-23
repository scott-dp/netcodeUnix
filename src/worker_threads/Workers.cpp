//
// Created by scott on 21.05.2025.
//


#include <mutex>
#include "../../include/worker_threads/Workers.h"

using namespace std;

Workers::Workers(int numThreads) {
    //Constructor
    this->numThreads = numThreads;
}

void Workers::post(function<void()> threadTask) {
    unique_lock<mutex> lock(taskMutex);
    tasks.emplace_back(threadTask);
    cv.notify_one();
}

void Workers::join() {
    //Join all threads
    stopRunning = true;
    cv.notify_all();
    for (auto &t : worker_threads) {
        cv.notify_one();
        t.join();
    }
}

void Workers::start() {
    for (int i = 0; i < numThreads; ++i) {
        worker_threads.emplace_back([this, i] {
            while(true) {
                function<void()> task;
                {
                    unique_lock<mutex> lock(this->taskMutex);
                    while (tasks.empty()) {
                        if (stopRunning) return;
                        cv.wait(lock);
                    }
                    //Now there is at least one task in tasks
                    task = *tasks.begin(); // Copy task for later use
                    tasks.pop_front();
                }
                task();
            }
        });
    }
}

