#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <queue>
#include <functional>

class ThreadPool {
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex mutex;
    std::condition_variable cv;
    bool shutdown = false;

public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->mutex);

                        // Wait until there is a task or shutdown is requested
                        this->cv.wait(lock, [this] {
                            return this->shutdown || !this->tasks.empty();
                        });

                        // If shutdown is requested and the queue is empty, exit the thread
                        if (this->shutdown && this->tasks.empty()) {
                            return;
                        }

                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> f) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (shutdown) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.push(std::move(f));
        }
        cv.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            shutdown = true;
        }
        cv.notify_all();
        for (auto& worker : threads) {
            worker.join();
        }
    }
};
