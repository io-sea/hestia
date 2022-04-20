#include "../object_store.h"

namespace hestia {
namespace obj {

/// @copydoc Object_store
class Disk : Object_store {
  public:
    /// @copydoc Object_store::Object_store
    Disk(){};
    /// @copydoc Object_store::Object_store
    ~Disk(){};

    /// @copydoc Object_store::put
    int put(
        const struct hsm_uint& oid, const void* buf, const std::size_t length);
};

}  // namespace obj
}  // namespace hestia
