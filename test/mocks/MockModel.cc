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

const MockOneToOneModel& MockModel::get_child() const
{
    return m_child.value();
}

MockModel& MockModel::operator=(const MockModel& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_my_field = other.m_my_field;
        m_child    = other.m_child;
        init();
    }
    return *this;
}

std::unique_ptr<ModelFactory> MockModel::create_factory()
{
    return std::make_unique<TypedModelFactory<MockModel>>();
}

std::string MockModel::get_type()
{
    return s_name;
}

void MockModel::init()
{
    m_name.set_index_scope(BaseField::IndexScope::PARENT);
    register_scalar_field(&m_my_field);
    register_one_to_one_proxy_field(&m_child);
}

MockOneToOneModel::MockOneToOneModel() : Model(s_name)
{
    init();
}

MockOneToOneModel::MockOneToOneModel(const std::string& id) : Model(id, s_name)
{
    init();
}

const std::string& MockOneToOneModel::get_field() const
{
    return m_my_field.get_value();
}

void MockOneToOneModel::set_field(const std::string& field)
{
    m_my_field.update_value(field);
}

MockOneToOneModel::MockOneToOneModel(const MockOneToOneModel& other) :
    Model(other)
{
    *this = other;
}

MockOneToOneModel& MockOneToOneModel::operator=(const MockOneToOneModel& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_my_field = other.m_my_field;
        m_parent   = other.m_parent;
        init();
    }
    return *this;
}

std::unique_ptr<ModelFactory> MockOneToOneModel::create_factory()
{
    return std::make_unique<TypedModelFactory<MockOneToOneModel>>();
}

std::string MockOneToOneModel::get_type()
{
    return s_name;
}

void MockOneToOneModel::init()
{
    m_name.set_index_scope(BaseField::IndexScope::PARENT);
    register_scalar_field(&m_my_field);
    register_foreign_key_field(&m_parent);
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

std::string MockManyToManyTargetModel::get_type()
{
    return s_name;
}


}  // namespace hestia::mock