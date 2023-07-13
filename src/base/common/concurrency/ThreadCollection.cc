#include "ThreadCollection.h"

#include "Logger.h"

#include <sstream>
#include <vector>

bool ThreadCollection::add(std::unique_ptr<std::thread> thread)
{
    if (!m_accepting) {
        return false;
    }

    std::stringstream sstr;
    sstr << "[" << thread->get_id() << "]";
    LOG_INFO("Registering thread: " + sstr.str());

    std::scoped_lock guard(m_mutex);
    m_threads[thread->get_id()] = std::move(thread);

    return true;
}

void ThreadCollection::join_and_clear_all()
{
    m_accepting = false;
    std::vector<std::thread*> threads;
    {
        std::scoped_lock guard(m_mutex);
        for (const auto& item : m_threads) {
            threads.push_back(item.second.get());
        }
    }

    for (auto thread : threads) {
        if (thread->joinable()) {
            thread->join();
        }
    }

    std::scoped_lock guard(m_mutex);
    threads.clear();
    m_threads.clear();
    m_accepting = true;
}

std::size_t ThreadCollection::size() const
{
    std::size_t size{0};
    {
        std::scoped_lock guard(m_mutex);
        size = m_threads.size();
    }
    return size;
}

void ThreadCollection::mark_for_removal(std::thread::thread::id input_id)
{
    std::scoped_lock guard(m_mutex);
    m_marked_for_removal.push_back(input_id);
}

void ThreadCollection::remove_marked()
{
    std::scoped_lock guard(m_mutex);
    for (const auto& id : m_marked_for_removal) {
        remove(id);
    }
    m_marked_for_removal.clear();
}

void ThreadCollection::remove(std::thread::thread::id input_id)
{
    if (auto const& it = m_threads.find(input_id); it != m_threads.end()) {
        std::stringstream sstr;
        sstr << "[" << input_id << "]";
        LOG_INFO("Removing thread: " + sstr.str());

        if (it->second->joinable()) {
            it->second->detach();
        }
        m_threads.erase(it);
    }
}