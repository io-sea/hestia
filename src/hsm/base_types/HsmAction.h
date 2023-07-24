#pragma once

#include "EnumUtils.h"
#include "HsmItem.h"
#include "OwnableModel.h"

#include <string>

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

    const std::string get_subject_key() const
    {
        return m_subject_key.get_value();
    };

    HsmItem::Type get_subject() const { return m_subject.get_value(); }

    Action get_action() const { return m_action.get_value(); }

    uint8_t get_source_tier() const { return m_source_tier.get_value(); }

    uint8_t get_target_tier() const { return m_target_tier.get_value(); }

    std::size_t get_offset() const { return m_offset.get_value(); }

    std::size_t get_size() const { return m_to_transfer.get_value(); }

    bool is_crud_method() const;

    bool is_data_management_action() const;

    bool is_data_io_action() const;

    bool is_data_put_action() const;

    bool has_action() const;

    void set_action(Action action);

    void set_source_tier(uint8_t tier);

    void set_target_tier(uint8_t tier);

    void set_offset(std::size_t offset) { m_offset.update_value(offset); }

    void set_subject_key(const std::string& key)
    {
        m_subject_key.update_value(key);
    }

    void set_size(const std::size_t& size) { m_to_transfer.update_value(size); }

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
    UIntegerField m_source_tier{"source_tier"};
    UIntegerField m_target_tier{"target_tier"};
    StringField m_subject_key{"subject_key"};
    BooleanField m_is_request{"is_request", false};

    static constexpr char trigger_migration_key[] = "trigger_migration";
};
}  // namespace hestia