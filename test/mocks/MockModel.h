#pragma once

#include "Model.h"
#include "RelationshipField.h"

namespace hestia::mock {
class MockModel : public Model {
  public:
    MockModel();

    MockModel(const std::string& id);

    MockModel(const MockModel& other);

    MockModel& operator=(const MockModel& other);

    static std::string get_type();

    static std::unique_ptr<ModelFactory> create_factory();

    void init();

    static constexpr char s_name[]{"mock_model"};
    StringField m_my_field{"my_field", "default_field"};
};

class MockModelWithParent : public Model {
  public:
    MockModelWithParent();

    MockModelWithParent(const std::string& id);

    MockModelWithParent(const MockModelWithParent& other);

    MockModelWithParent& operator=(const MockModelWithParent& other);

    static std::string get_type();

    static std::unique_ptr<ModelFactory> create_factory();

    void set_parent_id(const std::string& id) { m_parent.set_id(id); }

    void add_many_many_id(const std::string& id)
    {
        m_many_many_parents.add_id(id);
    };

    void init();

    static constexpr char s_mock_with_parent_name[]{"mock_model_with_parent"};
    StringField m_my_field{"my_field", "default_field"};

    ForeignKeyField m_parent{"parent", "mock_parent_model"};
    ManyToManyField m_many_many_parents{
        "many_many_parents", "mock_many_to_many_model"};
};

class MockParentModel : public Model {
  public:
    MockParentModel();

    MockParentModel(const std::string& id);

    MockParentModel(const MockParentModel& other);

    MockParentModel& operator=(const MockParentModel& other);

    static std::string get_type();

    static std::unique_ptr<ModelFactory> create_factory();

    const std::vector<MockModelWithParent>& get_models() const
    {
        return m_models.models();
    }

    void init();

    static constexpr char s_name[]{"mock_parent_model"};
    StringField m_my_field{"my_field", "default_field"};
    ForeignKeyProxyField<MockModelWithParent> m_models{"models"};
};

class MockManyToManyTargetModel : public Model {
  public:
    MockManyToManyTargetModel();

    MockManyToManyTargetModel(const std::string& id);

    MockManyToManyTargetModel(const MockManyToManyTargetModel& other);

    MockManyToManyTargetModel& operator=(
        const MockManyToManyTargetModel& other);

    static std::string get_type();

    static std::unique_ptr<ModelFactory> create_factory();

    const std::vector<MockModelWithParent>& get_many_to_many_children() const
    {
        return m_children.models();
    }

    void init();

    static constexpr char s_name[]{"mock_many_to_many_model"};
    ForeignKeyProxyField<MockModelWithParent> m_children{"children"};
};

}  // namespace hestia::mock