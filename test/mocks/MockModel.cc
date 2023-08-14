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
    m_name.set_index_scope(BaseField::IndexScope::PARENT);
    register_scalar_field(&m_my_field);
}

MockModelWithParent::MockModelWithParent() : Model(s_mock_with_parent_name)
{
    init();
}

MockModelWithParent::MockModelWithParent(const std::string& id) :
    Model(id, s_mock_with_parent_name)
{
    init();
}

MockModelWithParent::MockModelWithParent(const MockModelWithParent& other) :
    Model(other)
{
    *this = other;
}

MockModelWithParent& MockModelWithParent::operator=(
    const MockModelWithParent& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_my_field          = other.m_my_field;
        m_parent            = other.m_parent;
        m_many_many_parents = other.m_many_many_parents;
        init();
    }
    return *this;
}

std::unique_ptr<ModelFactory> MockModelWithParent::create_factory()
{
    return std::make_unique<TypedModelFactory<MockModelWithParent>>();
}

AdapterCollection::Ptr MockModelWithParent::create_adapters()
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

std::string MockModelWithParent::get_type()
{
    return s_mock_with_parent_name;
}

void MockModelWithParent::init()
{
    m_name.set_index_scope(BaseField::IndexScope::PARENT);
    register_scalar_field(&m_my_field);

    register_foreign_key_field(&m_parent);
    register_many_to_many_field(&m_many_many_parents);
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
    m_name.set_index_scope(BaseField::IndexScope::GLOBAL);
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

MockManyToManyTargetModel::MockManyToManyTargetModel() : Model(s_name)
{
    init();
}

MockManyToManyTargetModel::MockManyToManyTargetModel(const std::string& id) :
    Model(id, s_name)
{
    init();
}

MockManyToManyTargetModel::MockManyToManyTargetModel(
    const MockManyToManyTargetModel& other) :
    Model(other)
{
    *this = other;
}

MockManyToManyTargetModel& MockManyToManyTargetModel::operator=(
    const MockManyToManyTargetModel& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_children = other.m_children;
        init();
    }
    return *this;
}

void MockManyToManyTargetModel::init()
{
    m_name.set_index_scope(BaseField::IndexScope::GLOBAL);
    register_foreign_key_proxy_field(&m_children);
}

std::unique_ptr<ModelFactory> MockManyToManyTargetModel::create_factory()
{
    return std::make_unique<TypedModelFactory<MockManyToManyTargetModel>>();
}

AdapterCollection::Ptr MockManyToManyTargetModel::create_adapters()
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

std::string MockManyToManyTargetModel::get_type()
{
    return s_name;
}


}  // namespace hestia::mock