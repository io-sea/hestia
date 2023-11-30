#pragma once

#include "Dictionary.h"

#include <filesystem>
#include <sqlite3.h>

struct HestiaObject;

namespace hestia {
class PolicyEngine {
  public:
    PolicyEngine(
        const std::filesystem::path& cache_dir, const std::string& host = {});

    ~PolicyEngine();

    void initialize_db(bool clean = false);

    void log_db();

    int do_sync(std::time_t& sync_time);

    void start_event_listener(std::time_t last_sync_time);

  private:
    void on_events(std::time_t last_sync_time);

    void on_event(const Dictionary& event);

    void on_create(const Dictionary& event);

    void on_update(const Dictionary& event);

    void on_remove(const Dictionary& event);

    void on_read(const Dictionary& event);

    void on_hint(
        const std::string& object_id,
        const std::string& key,
        const std::string& value);

    void check_for_hints(const HestiaObject& object);

    int add_object_insert_values(uint8_t tier_index, std::string& statement);

    void add_tier_insert_values(uint8_t tier_index, std::string& statement);

    void replace_list_terminator(std::string& statement);

    void do_db_op(const std::string& statement);

    std::filesystem::path m_cache_dir;
    sqlite3* m_db{nullptr};
    std::time_t m_last_event_time{0};
};
}  // namespace hestia