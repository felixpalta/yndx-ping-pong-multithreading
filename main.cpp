#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class semaphore
{
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_;

public:
    explicit semaphore( int i = 0) : count_(i) {}
    void notify() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    void wait() {
        std::unique_lock<decltype(mutex_)> lock(mutex_);
        while(!count_) // Handle spurious wake-ups.
            condition_.wait(lock);
        --count_;
    }

    bool try_wait() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        if(count_) {
            --count_;
            return true;
        }
        return false;
    }
};

void print(std::string name) {
    std::cout << name << std::endl;
}

void printLoop(std::string name, semaphore * first, semaphore * second, std::shared_future<void> * futureObj) {
    while(futureObj->wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
        first->wait();
        print(name);
        second->notify();
    }
}

int main()
{
    std::promise<void> exitSignal;
    auto futureObj = std::shared_future<void>(exitSignal.get_future());
    semaphore pingSem(1);
    semaphore pongSem(0);
    std::thread t1(printLoop, "Ping!", &pingSem, &pongSem, &futureObj);
    std::thread t2(printLoop, "Pong!", &pongSem, &pingSem, &futureObj);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    exitSignal.set_value();
    t1.join();
    t2.join();
    return 0;
}
