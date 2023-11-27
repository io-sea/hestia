#pragma once

#include "Dictionary.h"

#include <filesystem>
#include <sqlite3.h>

namespace hestia {
class PolicyEngine {
  public:
    PolicyEngine(const std::filesystem::path& cache_dir);

    ~PolicyEngine();

    void initialize_db();

    void do_initial_sync();

    void start_event_listener();

  private:
    void on_events();

    void on_event(const Dictionary& event);

    void on_create(const Dictionary& event);

    void on_update(const Dictionary& event);

    void on_remove(const Dictionary& event);

    void on_read(const Dictionary& event);

    void on_hint();

    void do_db_op(const std::string& statement);

    std::filesystem::path m_cache_dir;
    sqlite3* m_db{nullptr};
    std::time_t m_last_event_time{0};
};
}  // namespace hestia