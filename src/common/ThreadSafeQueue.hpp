#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

template <typename T>
class ThreadSafeQueue {
  public:
      void push(T);
      bool pop(T&);
      void stop();
      bool isStoped();
  private:
      std::queue<T> m_Queue;
      std::mutex   m_Mutex;
      std::condition_variable m_Condition;
      bool m_Stopping = false;
};

template <typename T>
void ThreadSafeQueue<T>::push(T el) {
    {
        std::lock_guard lock(m_Mutex);
        m_Queue.push(std::move(el));
    }
    m_Condition.notify_one();
}

template <typename T>
bool ThreadSafeQueue<T>::pop(T& el) {
    std::unique_lock lock(m_Mutex);
    std::cout << m_Queue.size() << std::endl;
    m_Condition.wait(lock, [this] {
        return !m_Queue.empty() || m_Stopping;
    });

    if (m_Queue.empty())
        return false;

    el = std::move(m_Queue.front());
    m_Queue.pop();
    return true;
}

template <typename T>
void ThreadSafeQueue<T>::stop() {
    {
        std::lock_guard lock(m_Mutex);
        m_Stopping = true;
    }
    m_Condition.notify_all();
}

template <typename T>
bool ThreadSafeQueue<T>::isStoped() {
    std::lock_guard lock(m_Mutex);
    return m_Stopping;
}

