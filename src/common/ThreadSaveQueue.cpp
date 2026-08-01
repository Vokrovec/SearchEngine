#include "ThreadSaveQueue.hpp"
#include <mutex>

template <typename T>
void ThreadSaveQueue<T>::push(T el) {
    {
        std::lock_guard lock(m_Mutex);
        m_Queue.push(std::move(el));
    }
    m_Condition.notify_one();
}

template <typename T>
bool ThreadSaveQueue<T>::pop(T& el) {
    std::unique_lock lock(m_Mutex);

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
void ThreadSaveQueue<T>::stop() {
    {
        std::lock_guard lock(m_Mutex);
        m_Stopping = true;
    }
    m_Condition.notify_all();
}
