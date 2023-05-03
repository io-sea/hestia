#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

class ThreadCollection {
  public:
    bool add(std::unique_ptr<std::thread> thread);

    void join_and_clear_all();

    void mark_for_removal(std::thread::thread::id input_id);

    void remove_marked();

    std::size_t size() const;

  private:
    void remove(std::thread::thread::id input_id);

    mutable std::mutex m_mutex;
    std::atomic<bool> m_accepting{true};

    std::vector<std::thread::thread::id> m_marked_for_removal;
    std::unordered_map<std::thread::thread::id, std::unique_ptr<std::thread>>
        m_threads;
};