#include "MockModel.h"

#include "CrudAttributes.h"

namespace hestia::mock {
MockModel::MockModel() : Model(s_name)
{
    init();
}

MockModel::MockModel(const std::string& id) : Model(id, s_name)
{
    init();
}

MockModel::MockModel(const MockModel& other) : Model(other)
{
    *this = other;
}

MockModel& MockModel::operator=(const MockModel& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_my_field = other.m_my_field;
        m_parent   = other.m_parent;
        init();
    }
    return *this;
}

std::unique_ptr<ModelFactory> MockModel::create_factory()
{
    return std::make_unique<TypedModelFactory<MockModel>>();
}

AdapterCollection::Ptr MockModel::create_adapters()
{
    auto model_factory     = create_factory();
    auto model_factory_raw = model_factory.get();

    auto adapters =
        std::make_unique<AdapterCollection>(std::move(model_factory));
    adapters->add_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::JSON),
        std::make_unique<JsonAdapter>(model_factory_raw));
    adapters->add_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::KEY_VALUE),
        std::make_unique<KeyValueAdapter>(model_factory_raw));

    return adapters;
}

std::string MockModel::get_type()
{
    return s_name;
}

void MockModel::init()
{
    m_name.set_index_on(true);
    register_scalar_field(&m_my_field);
    register_named_foreign_key_field(&m_parent);
}

MockParentModel::MockParentModel() : Model(s_name)
{
    init();
}

MockParentModel::MockParentModel(const std::string& id) : Model(id, s_name)
{
    init();
}

MockParentModel::MockParentModel(const MockParentModel& other) : Model(other)
{
    *this = other;
}

MockParentModel& MockParentModel::operator=(const MockParentModel& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_my_field = other.m_my_field;
        m_models   = other.m_models;
        init();
    }
    return *this;
}

void MockParentModel::init()
{
    m_name.set_index_on(true);
    register_scalar_field(&m_my_field);
    register_foreign_key_proxy_field(&m_models);
}

std::unique_ptr<ModelFactory> MockParentModel::create_factory()
{
    return std::make_unique<TypedModelFactory<MockParentModel>>();
}

AdapterCollection::Ptr MockParentModel::create_adapters()
{
    auto model_factory     = create_factory();
    auto model_factory_raw = model_factory.get();

    auto adapters =
        std::make_unique<AdapterCollection>(std::move(model_factory));
    adapters->add_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::JSON),
        std::make_unique<JsonAdapter>(model_factory_raw));
    adapters->add_adapter(
        CrudAttributes::to_string(CrudAttributes::Format::KEY_VALUE),
        std::make_unique<KeyValueAdapter>(model_factory_raw));

    return adapters;
}

std::string MockParentModel::get_type()
{
    return s_name;
}


}  // namespace hestia::mock