#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class LockQueue
{
public:
    // 多个worker线程都会写日志queue 
    void Push(const T &data)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(data);
        cond.notify_one();
    }
     // 一个线程读日志queue，写日志文件
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_queue.empty())
        {
            cond.wait(lock);
        }
        T data = m_queue.front();
        m_queue.pop();
        return data;

    }



private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable cond;
};