#pragma once

#include "EnumUtils.h"
#include "HsmItem.h"
#include "OwnableModel.h"

#include <string>
#include <vector>

namespace hestia {
class HsmAction : public HsmItem, public OwnableModel {
  public:
    STRINGABLE_ENUM(
        Action,
        NONE,
        CRUD,
        PUT_DATA,
        GET_DATA,
        COPY_DATA,
        MOVE_DATA,
        RELEASE_DATA)
    STRINGABLE_ENUM(Status, RUNNING, FINISHED_OK, ERROR)

    HsmAction();

    HsmAction(HsmItem::Type subject, HsmAction::Action action);

    HsmAction(const HsmAction& other);

    static std::string get_type();

    const std::string& get_subject_key() const;

    static std::vector<std::string> get_action_subjects();

    static std::vector<HsmAction::Action> get_subject_actions(
        const std::string& subject);

    HsmItem::Type get_subject() const;

    Action get_action() const;

    std::string get_action_as_string() const;

    uint8_t get_source_tier() const;

    uint8_t get_target_tier() const;

    const std::string& get_source_tier_id() const;

    const std::string& get_target_tier_id() const;

    std::size_t get_offset() const;

    std::size_t get_size() const;

    std::size_t get_progress_interval() const;

    std::size_t get_num_transferred() const;

    const std::string& get_preffered_node_address() const;

    Status get_status() const;

    bool has_source_tier_id() const;

    bool has_target_tier_id() const;

    bool is_crud_method() const;

    bool is_data_management_action() const;

    bool is_data_io_action() const;

    bool is_data_put_action() const;

    void on_error(const std::string& message);

    void on_finished_ok(std::size_t bytes_transferred);

    bool has_action() const;

    void set_action(Action action);

    void set_source_tier(uint8_t tier);

    void set_target_tier(uint8_t tier);

    void set_source_tier_id(const std::string& id);

    void set_target_tier_id(const std::string& id);

    void set_offset(std::size_t offset);

    void set_subject_key(const std::string& key);

    void set_subject(HsmItem::Type subject);

    void set_size(std::size_t size);

    void set_num_transferred(std::size_t size);

    void set_preferred_node_address(const std::string& addr);

    HsmAction& operator=(const HsmAction& other);

  private:
    void init();

    EnumField<Action, Action_enum_string_converter> m_action{"action"};
    EnumField<HsmItem::Type, HsmItem::Type_enum_string_converter> m_subject{
        "subject"};
    EnumField<Status, Status_enum_string_converter> m_status{"status"};
    UIntegerField m_to_transfer{"to_transfer"};
    UIntegerField m_offset{"offset"};
    UIntegerField m_transferred{"transferred"};
    UIntegerField m_source_tier{
        "source_tier"}; /*!< The source tier - referenced by priority */
    UIntegerField m_target_tier{
        "target_tier"}; /*!< The target tier - referenced by priority */
    StringField m_source_tier_id{
        "source_tier_id"}; /*!< The source tier - referenced by unique
                              identifier */
    StringField m_target_tier_id{
        "target_tier_id"}; /*!< The target tier - referenced by unique
                              identifier */
    UIntegerField m_progress_interval{"progess_interval", 0};
    StringField m_subject_key{"subject_key"};
    StringField m_preferred_node_address{"preferred_node_address"};
    StringField m_status_message{"status_message"};
    BooleanField m_is_request{"is_request", false};

    static constexpr char trigger_migration_key[] = "trigger_migration";
};
}  // namespace hestia