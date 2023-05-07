#include "S3Client.h"

#include "ProjectConfig.h"

#ifdef HAS_LIBS3
#include "LibS3InterfaceImpl.h"
#endif

#include "S3Object.h"

namespace hestia {
S3Client::S3Client(IS3InterfaceImpl::Ptr impl) : m_impl(std::move(impl))
{
#ifdef HAS_LIBS3
    if (!m_impl) {
        m_impl = std::make_unique<LibS3InterfaceImpl>();
    }
#endif
}

S3Client::Ptr S3Client::create(IS3InterfaceImpl::Ptr impl)
{
    return std::make_unique<S3Client>(std::move(impl));
}

S3Client::Ptr S3Client::create(
    const S3Config& config, IS3InterfaceImpl::Ptr impl)
{
    auto instance = create(std::move(impl));
    instance->initialize(config);
    return instance;
}

std::string S3Client::get_registry_identifier()
{
    return hestia::project_config::get_project_name() + "::S3Client";
}

void S3Client::initialize(const S3Config& config)
{
    m_impl->initialize(config);
}

void S3Client::put(
    const StorageObject& object, const Extent& extent, Stream* stream) const
{
    (void)object;
    (void)extent;
    (void)stream;

    // m_impl->put(object, extent, stream);
}

void S3Client::get(
    StorageObject& object, const Extent& extent, Stream* stream) const
{
    (void)object;
    (void)extent;
    (void)stream;
    // m_impl->get(object, extent, stream);
}

void S3Client::remove(const StorageObject& obj) const
{
    (void)obj;
    // m_impl->remove(obj);
}

void S3Client::list(
    const Metadata::Query& query, std::vector<StorageObject>& found) const
{
    (void)query;
    (void)found;
}

bool S3Client::exists(const StorageObject& object) const
{
    (void)object;
    return false;
}
}  // namespace hestia