#pragma once
#include <queue>
#include <map>
#include <set>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace ThreadSafe {
    
    template <typename T>
    class Queue {
      public:
        void push(T el) {
            {
                std::lock_guard lock(m_Mutex);
                m_Queue.push(std::move(el));
            }
            m_Condition.notify_one();
        }
        void push_vec(std::vector<T> v_el) {
            {
                std::lock_guard lock(m_Mutex);
                m_Queue.reserve(v_el.size());
                for (auto& el: v_el)
                    m_Queue.push(std::move(el));
            }
            m_Condition.notify_one();
        }
        bool pop(T& el) {
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

        void stop() {
            {
              std::lock_guard lock(m_Mutex);
              m_Stopping = true;
            }
            m_Condition.notify_all();
        }
        bool isStoped() {
            std::lock_guard lock(m_Mutex);
            return m_Stopping;
        }
      private:
        std::queue<T> m_Queue;
        std::mutex   m_Mutex;
        std::condition_variable m_Condition;
        bool m_Stopping = false;
    };


    template <typename K, typename V>
    class Map {
    public:
        Map() = default;

        // Insert or overwrite
        void insert(K key, V value) {
            std::lock_guard lock(m_Mutex);
            m_Map[std::move(key)] = std::move(value);
        }

        template <typename... Args>
        V& emplace(K key, Args&&... args) {
            std::lock_guard lock(m_Mutex);

            auto [it, inserted] = m_Map.try_emplace(
                std::move(key),
                std::forward<Args>(args)...
            );

            return it->second;
        }

        // Check if key exists
        bool contains(const K& key) const {
            std::lock_guard lock(m_Mutex);
            return m_Map.contains(key);
        }

        // Get copy of value
        std::optional<V> get(const K& key) const {
            std::lock_guard lock(m_Mutex);

            auto it = m_Map.find(key);

            if (it == m_Map.end())
                return std::nullopt;

            return it->second;
        }

        // Remove element
        bool erase(const K& key) {
            std::lock_guard lock(m_Mutex);
            return m_Map.erase(key) > 0;
        }

        size_t size() const {
            std::lock_guard lock(m_Mutex);
            return m_Map.size();
        }

    private:
        mutable std::mutex m_Mutex;
        std::map<K, V> m_Map;
    };

    template <typename T>
    class Set {
        public:
            Set() = default;

            // Insert element. Returns true if inserted, false if already exists.
            bool insert(T value) {
                std::lock_guard lock(m_Mutex);
                return m_Set.insert(std::move(value)).second;
            }

            // Check if element exists
            bool contains(const T& value) const {
                std::lock_guard lock(m_Mutex);
                return m_Set.contains(value);
            }

            // Remove element
            bool erase(const T& value) {
                std::lock_guard lock(m_Mutex);
                return m_Set.erase(value) > 0;
            }

            // Number of elements
            size_t size() const {
                std::lock_guard lock(m_Mutex);
                return m_Set.size();
            }

            // Remove everything
            void clear() {
                std::lock_guard lock(m_Mutex);
                m_Set.clear();
            }

        private:
            mutable std::mutex m_Mutex;
            std::set<T> m_Set;
    };
}


