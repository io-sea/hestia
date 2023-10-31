#pragma once

#include "EnumUtils.h"
#include "Model.h"

#include <array>
#include <string>
#include <vector>

namespace hestia {
class HsmItem {
  public:
    STRINGABLE_ENUM(
        Type,
        DATASET,
        OBJECT,
        ACTION,
        NAMESPACE,
        TIER,
        EXTENT,
        METADATA,
        OBJECT_STORE_BACKEND,
        NODE,
        UNKNOWN)

    // NOTE: These must be identical to the STRINGABLE_ENUM outputs (i.e. lower
    // case versions of the num name) They are used for performance as
    // STRINGABLE_ENUM string conversion is relatively slow.
    static constexpr char hsm_object_name[]{"object"};
    static constexpr char user_metadata_name[]{"metadata"};
    static constexpr char tier_extents_name[]{"extent"};
    static constexpr char tier_name[]{"tier"};
    static constexpr char dataset_name[]{"dataset"};
    static constexpr char namespace_name[]{"namespace"};
    static constexpr char hsm_action_name[]{"action"};
    static constexpr char hsm_node_name[]{"node"};
    static constexpr char object_store_backend_name[]{"object_store_backend"};

    HsmItem(Type type);

    Type get_hsm_type() const;

    static std::string to_name(Type type);

    static Type from_name(const std::string& type_name);

    static std::array<Type, 9> get_all_items();

    static std::vector<std::string> get_hsm_subjects();

  protected:
    Type m_hsm_type{Type::OBJECT};
    static constexpr std::array<Type, 9> s_all_items{
        Type::DATASET,   Type::OBJECT, Type::ACTION,
        Type::NAMESPACE, Type::TIER,   Type::EXTENT,
        Type::METADATA,  Type::NODE,   Type::OBJECT_STORE_BACKEND};
};

class HsmItemModel : public HsmItem, public Model {};

}  // namespace hestia