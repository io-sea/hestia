#include "../../src/kv_store.h"
#include <filesystem>

namespace hestia {
namespace kv {

/// @copydoc Kv_store
class Disk : Kv_store {
  public:
    /// @copydoc Kv_store::Kv_store
    Disk()
    {
        if (!m_store.exists()) {
            std::filesystem::create_directory(m_store.path());
        }
    };

    /// @copydoc Kv_store::Kv_store
    ~Disk(){};

    /// @copydoc Kv_store::object_exists
    bool object_exists(const struct hsm_uint& oid);

    /// @copydoc Kv_store::put_meta_data
    //    int put_meta_data(const struct hsm_obj& obj);

    /// @copydoc Kv_store::put_meta_data
    int put_meta_data(const struct hsm_uint& oid, const nlohmann::json& attrs);

    /// @copydoc Kv_store::get_meta_data
    int get_meta_data(struct hsm_obj& obj);

    ///@copydoc kv_store::get_meta_data
    nlohmann::json get_meta_data(const struct hsm_uint& oid);

    ///@copydoc kv_store::get_meta_data
    nlohmann::json get_meta_data(
        const struct hsm_uint& oid, const std::string& attr_keys);

    /// @copydoc Kv_store::remove
    int remove(const struct hsm_uint& oid);

    /// @copydoc Kv_store::list
    std::vector<struct hsm_uint> list(const std::uint8_t tier = 0);

    std::string get_path() const { return m_store.path().string(); }

    char get_delim() const { return m_delim; }

  private:
    const char m_delim = ';';

    const std::filesystem::directory_entry m_store{"kv_store"};

    std::string get_filename_from_oid(const struct hsm_uint& oid) const
    {
        const std::string filename{
            std::to_string(oid.higher) + '-' + std::to_string(oid.lower)
            + ".meta"};
        const auto path = m_store.path() / filename;
        return path.string();
    }
};

}  // namespace kv
}  // namespace hestia
