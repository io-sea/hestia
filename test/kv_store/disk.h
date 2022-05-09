#include "../../src/kv_store.h"
#include <nlohmann/json.hpp>

namespace hestia {
namespace kv {

/// @copydoc Kv_store
class Disk : Kv_store {
  public:
    /// @copydoc Kv_store::Kv_store
    Disk(){};
    /// @copydoc Kv_store::Kv_store
    ~Disk(){};

    /// @copydoc Kv_store::object_exists
    bool object_exists(const struct hsm_uint& oid);

    /// @copydoc Kv_store::put_meta_data
    int put_meta_data(const struct hsm_obj& obj);

    /// @copydoc Kv_store::put_meta_data
    int put_meta_data(const struct hsm_uint& oid, const nlohmann::json& attrs);

    /// @copydoc Kv_store::get_meta_data
    int get_meta_data(struct hsm_obj& obj);
};

}  // namespace kv
}  // namespace hestia
