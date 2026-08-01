#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSaveQueue {
  public:
      void push(T);
      bool pop(T&);
      void stop();
  private:
      std::queue<T> m_Queue;
      std::mutex   m_Mutex;
      std::condition_variable m_Condition;
      bool m_Stopping;
};
