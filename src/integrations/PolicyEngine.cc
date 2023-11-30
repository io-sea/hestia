#include "PolicyEngine.h"

#include "Logger.h"
#include "hestia_iosea.h"

#include "StringUtils.h"
#include "TimeUtils.h"
#include "YamlUtils.h"

#include <algorithm>
#include <chrono>
#include <thread>

namespace hestia {
PolicyEngine::PolicyEngine(
    const std::filesystem::path& cache_dir, const std::string& host) :
    m_cache_dir(cache_dir)
{
    if (host.empty()) {
        hestia_initialize(nullptr, nullptr, nullptr);
    }
    else {
        std::string config =
            "{\"server\" : {\"controller_address\" : \"" + host + "\"}}";
        std::cout << "starting with config: " << config << std::endl;
        hestia_initialize(nullptr, nullptr, config.c_str());
    }
}

PolicyEngine::~PolicyEngine()
{
    if (m_db != nullptr) {
        sqlite3_close(m_db);
    }

    hestia_finish();
}

static int db_callback(void*, int argc, char** argv, char** column_name)
{
    for (int i = 0; i < argc; i++) {
        std::cout << column_name[i] << " | " << argv[i] << std::endl;
    }
    return 0;
}

void PolicyEngine::do_db_op(const std::string& statement)
{
    LOG_INFO("Running statement: " << statement);
    char* error_msg = nullptr;
    int rc = sqlite3_exec(m_db, statement.c_str(), db_callback, 0, &error_msg);
    if (rc != SQLITE_OK) {
        if (error_msg == nullptr) {
            LOG_ERROR("Unknown sql error");
        }
        else {
            LOG_ERROR("SQL error: %s\n" << error_msg);
        }
        sqlite3_free(error_msg);
    }
}

void PolicyEngine::initialize_db(bool clean)
{
    auto db_path = m_cache_dir / std::filesystem::path("policy_engine_db.db");
    if (clean && std::filesystem::exists(db_path)) {
        std::filesystem::remove(db_path);
    }

    int rc = ::sqlite3_open(db_path.c_str(), &m_db);
    if (rc != 0) {
        LOG_ERROR("Failed to open db");
    }

    if (clean) {
        // Create objects table
        std::string statement = R"(
            CREATE TABLE IF NOT EXISTS objects (
                id TEXT PRIMARY KEY,
                modified_time INT NOT NULL,
                created_time INT NOT NULL,
                accessed_time INT NOT NULL,
                size INT NOT NULL
            );
        )";
        do_db_op(statement);

        // Create tiers table
        statement = R"(
            CREATE TABLE IF NOT EXISTS tiers (
                priority INT PRIMARY KEY
            );
        )";
        do_db_op(statement);
    }
}

std::string get_object_columns()
{
    return "id, modified_time, created_time, accessed_time, size";
}

void PolicyEngine::log_db()
{
    std::string tier_read_statement = R"(SELECT priority FROM tiers)";
    std::cout << "Tiers" << std::endl;
    do_db_op(tier_read_statement);

    std::string object_read_statement =
        "SELECT " + get_object_columns() + " FROM objects";
    std::cout << "Objects" << std::endl;
    do_db_op(object_read_statement);
}

std::string get_object_insert_values(const HestiaObject& obj)
{
    std::string ret;
    ret += "('" + std::string(obj.m_uuid) + "',";
    ret += std::to_string(obj.m_last_modified_time) + ",";
    ret += std::to_string(obj.m_creation_time) + ",";
    ret += std::to_string(obj.m_last_accessed_time) + ",";
    ret += std::to_string(obj.m_size) + ")";
    return ret;
}

std::string get_object_update_values(const HestiaObject& obj)
{
    std::string ret;
    ret +=
        "modified_time = " + std::to_string(obj.m_last_modified_time) + ",\n";
    ret += "created_time = " + std::to_string(obj.m_creation_time) + ",\n";
    ret +=
        "accessed_time = " + std::to_string(obj.m_last_accessed_time) + ",\n";
    ret += "size = " + std::to_string(obj.m_size) + "\n";
    return ret;
}

std::string get_object_update_values_for_read(const std::string& time)
{
    std::string ret;
    ret += "accessed_time = " + time + "\n";
    return ret;
}

int PolicyEngine::add_object_insert_values(
    uint8_t tier_index, std::string& statement)
{
    HestiaId* object_ids{nullptr};
    std::size_t num_obj_ids{0};
    auto rc = hestia_object_list(tier_index, &object_ids, &num_obj_ids);
    if (rc != 0) {
        LOG_ERROR("Failed to list objects on tier");
        return rc;
    }

    for (std::size_t jdx = 0; jdx < num_obj_ids; jdx++) {
        HestiaObject object;
        rc = hestia_object_get_attrs(&object_ids[jdx], &object);
        if (rc != 0) {
            LOG_ERROR("Failed to get object");
            return rc;
        }
        statement += get_object_insert_values(object) + ",\n";
        hestia_init_object(&object);
    }
    // hestia_free_ids(&object_ids, num_obj_ids);
    return 0;
}

void PolicyEngine::add_tier_insert_values(
    uint8_t tier_index, std::string& statement)
{
    statement += "(" + std::to_string(tier_index) + "),\n";
}

void PolicyEngine::replace_list_terminator(std::string& statement)
{
    if (statement[statement.size() - 2] == ',') {
        statement[statement.size() - 2] = ';';
    }
}

int PolicyEngine::do_sync(std::time_t& sync_time)
{
    // Get tiers
    uint8_t* tiers{nullptr};
    std::size_t num_tiers{0};
    if (const auto rc = hestia_list_tiers(&tiers, &num_tiers); rc != 0) {
        LOG_ERROR("Failed to list tiers");
        return rc;
    }

    // Add them to db
    std::string tier_insert_stmt = R"(
        INSERT INTO tiers (priority)
        VALUES
    )";
    for (std::size_t idx = 0; idx < num_tiers; idx++) {
        add_tier_insert_values(tiers[idx], tier_insert_stmt);
    }
    replace_list_terminator(tier_insert_stmt);
    do_db_op(tier_insert_stmt);

    // Get objects and add to db
    std::string obj_insert_stmt =
        "INSERT INTO objects (" + get_object_columns() + ") VALUES ";

    bool objects_found{false};
    for (std::size_t idx = 0; idx < num_tiers; idx++) {
        std::string tier_object_values;
        auto rc = add_object_insert_values(tiers[idx], tier_object_values);
        if (rc != 0) {
            return rc;
        }
        if (!tier_object_values.empty()) {
            objects_found = true;
        }
        obj_insert_stmt += tier_object_values;
    }

    sync_time = TimeUtils::get_current_time();

    if (objects_found) {
        replace_list_terminator(obj_insert_stmt);
        do_db_op(obj_insert_stmt);
    }
    hestia_free_tier_ids(&tiers);
    return 0;
}

void PolicyEngine::start_event_listener(std::time_t last_sync_time)
{
    // Since we are building mostly on mac - use a simple poll for now
    // to keep things cross platform.
    std::filesystem::file_time_type event_feed_last_modified_time{};
    bool first_pass{true};
    const auto event_feed_path =
        m_cache_dir / std::filesystem::path("event_feed.yaml");

    while (true) {
        bool have_events = false;
        if (std::filesystem::exists(event_feed_path)) {
            const auto last_write_time_fs =
                std::filesystem::last_write_time(event_feed_path);
            if (first_pass) {
                have_events                   = true;
                first_pass                    = false;
                event_feed_last_modified_time = last_write_time_fs;
            }
            else if (last_write_time_fs > event_feed_last_modified_time) {
                std::time_t last_write =
                    last_write_time_fs.time_since_epoch().count();
                LOG_INFO("Last write time: " << last_write);
                event_feed_last_modified_time = last_write_time_fs;
                have_events                   = true;
            }
        }
        if (have_events) {
            on_events(last_sync_time);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void PolicyEngine::on_events(std::time_t last_sync_time)
{
    const auto event_feed_path =
        m_cache_dir / std::filesystem::path("event_feed.yaml");

    std::vector<Dictionary::Ptr> events;
    YamlUtils::load_all(event_feed_path.string(), events);

    std::time_t file_last_event = m_last_event_time;
    for (const auto& root : events) {
        auto event = root->get_map_item("root");
        const auto event_time =
            std::stoll(event->get_map_item("time")->get_scalar());
        if (event_time > m_last_event_time && event_time > last_sync_time) {
            if (event_time > file_last_event) {
                file_last_event = event_time;
            }
            on_event(*event);
        }
    }
    m_last_event_time = file_last_event;
}

void PolicyEngine::on_event(const Dictionary& event)
{
    if (event.get_tag().second == "!create") {
        on_create(event);
    }
    else if (event.get_tag().second == "!read") {
        on_read(event);
    }
    else if (event.get_tag().second == "!update") {
        on_update(event);
    }
    else if (event.get_tag().second == "!remove") {
        on_remove(event);
    }
}

void PolicyEngine::on_create(const Dictionary& event)
{
    LOG_INFO("On create");

    std::string obj_insert_stmt =
        "INSERT INTO objects (" + get_object_columns() + ") VALUES ";

    HestiaId object_id;
    hestia_init_id(&object_id);

    const auto object_uuid = event.get_map_item("id")->get_scalar();
    StringUtils::to_char(object_uuid, &object_id.m_uuid);

    HestiaObject object;
    auto rc = hestia_object_get_attrs(&object_id, &object);
    delete[] object_id.m_uuid;
    if (rc != 0) {
        LOG_ERROR("Failed to get object");
        return;
    }

    obj_insert_stmt += get_object_insert_values(object) + ";";
    hestia_init_object(&object);
    do_db_op(obj_insert_stmt);
}


void PolicyEngine::on_update(const Dictionary& event)
{
    LOG_INFO("On update");

    const auto object_uuid = event.get_map_item("id")->get_scalar();

    HestiaId object_id;
    hestia_init_id(&object_id);
    StringUtils::to_char(object_uuid, &object_id.m_uuid);

    HestiaObject object;
    auto rc = hestia_object_get_attrs(&object_id, &object);
    delete[] object_id.m_uuid;
    if (rc != 0) {
        LOG_ERROR("Failed to get object");
        return;
    }

    std::string obj_update_stmt =
        "UPDATE objects SET " + get_object_update_values(object);

    obj_update_stmt += " WHERE id = '" + object_uuid + "';";
    do_db_op(obj_update_stmt);

    check_for_hints(object);

    hestia_init_object(&object);
}

void PolicyEngine::check_for_hints(const HestiaObject& object)
{
    std::vector<std::string> known_hints{"trigger_migration"};

    for (std::size_t idx = 0; idx < object.m_num_attrs; idx++) {
        std::string key = object.m_attrs[idx].m_key;
        if (std::find(known_hints.begin(), known_hints.end(), key)
            != known_hints.end()) {
            on_hint(object.m_uuid, key, object.m_attrs[idx].m_value);
        }
    }
}

void PolicyEngine::on_read(const Dictionary& event)
{
    LOG_INFO("On read");

    const auto object_uuid = event.get_map_item("id")->get_scalar();
    const auto read_time   = event.get_map_item("time")->get_scalar();
    std::string obj_update_stmt =
        "UPDATE objects SET " + get_object_update_values_for_read(read_time);

    obj_update_stmt += " WHERE id = '" + object_uuid + "';";
    do_db_op(obj_update_stmt);
}

void PolicyEngine::on_remove(const Dictionary& event)
{
    LOG_INFO("On remove");

    const auto object_uuid = event.get_map_item("id")->get_scalar();

    std::string obj_delete_stmt =
        "DELETE FROM objects WHERE id = '" + object_uuid + "';";
    do_db_op(obj_delete_stmt);
}

void PolicyEngine::on_hint(
    const std::string& object_id,
    const std::string& key,
    const std::string& value)
{
    if (key == "trigger_migration") {
        if (value.empty()) {
            return;
        }

        // Remove the hint
        HestiaId id;
        hestia_init_id(&id);
        StringUtils::to_char(object_id, &id.m_uuid);

        auto kv_pairs = new HestiaKeyValuePair[1];
        StringUtils::to_char(key, &kv_pairs[0].m_key);
        StringUtils::to_char("", &kv_pairs[0].m_value);
        auto rc = hestia_object_set_attrs(&id, kv_pairs, 1);
        if (rc != 0) {
            LOG_ERROR("Error getting object attributes: " + object_id);
            return;
        }

        delete[] kv_pairs[0].m_key;
        delete[] kv_pairs[0].m_value;
        delete[] kv_pairs;

        // Complete the action
        const auto& [src, tgt] = StringUtils::split_on_first(value, ",");
        hestia_object_move(&id, std::stoul(src), std::stoul(tgt));

        delete[] id.m_uuid;
    }
}
}  // namespace hestia