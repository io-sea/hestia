#include "object_store.h"
#include <filesystem>

namespace hestia {
namespace obj {

/// @copydoc Object_store
class Disk : Object_store {
  public:
    /// @copydoc Object_store::Object_store
    Disk()
    {
        if (!m_store.exists()) {
            std::filesystem::create_directories(m_store.path());
        }
    };
    /// @copydoc Object_store::Object_store
    ~Disk(){};

    /// @copydoc Object_store::put
    int put(
        const struct hsm_uint& oid,
        const void* buf,
        const std::size_t off,
        const std::size_t length,
        const std::uint8_t target_tier = 0);

    /// @copydoc Object_store::get
    int get(
        const struct hsm_uint& oid,
        void* buf,
        const std::size_t off,
        const std::size_t length,
        const std::uint8_t src_tier = 0);

    /// @copydoc Object_store::remove
    int remove(const struct hsm_uint& oid, const std::uint8_t tier = 0);
    /// @copydoc Object_store::copy
    int copy(
        const struct hsm_uint& oid,
        const std::uint8_t src_tier,
        const std::uint8_t tgt_tier);

    /// @copydoc Object_store::move
    int move(
        const struct hsm_uint& oid,
        const std::uint8_t src_tier,
        const std::uint8_t tgt_tier);

    std::string get_path() const { return m_store.path().string(); }

    std::string get_tier_path(const std::uint8_t tier = 0) const
    {
        return (m_store.path() / ("tier" + std::to_string(tier))).string();
    }

  private:
    const std::filesystem::directory_entry m_store{"object_store"};

    std::string get_filename_from_oid(
        const struct hsm_uint& oid, const std::uint8_t tier = 0) const
    {
        /* concatenates paths */
        const std::filesystem::directory_entry tier_dir{
            m_store.path() / ("tier" + std::to_string(tier))};

        if (!tier_dir.exists()) {
            std::filesystem::create_directory(tier_dir.path());
        }

        const std::string filename{
            std::to_string(oid.higher) + std::to_string(oid.lower) + ".data"};
        const auto path = tier_dir.path() / filename;
        return path.string();
    }
};

}  // namespace obj
}  // namespace hestia
