#include "../../src/kv_store.h"

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

    /// @copydoc Kv_store::remove
    int remove(const struct hsm_uint& oid);

    /// @copydoc Kv_store::list
    std::vector<struct hsm_uint> list(const std::uint8_t tier = 0);

  private:
    const char m_delim = ';';

    std::string get_filename_from_oid(const struct hsm_uint& oid) const
    {
        return std::to_string(oid.higher) + '-' + std::to_string(oid.lower)
               + ".md";
    }
};

}  // namespace kv
}  // namespace hestia
