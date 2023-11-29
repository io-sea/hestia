#pragma once

#include "RelationshipField.h"
#include "SerializeableWithFields.h"

#include <memory>
#include <stdexcept>

namespace hestia {
class Model : public SerializeableWithFields {
  public:
    struct ForeignKeyContext {
        std::string m_type;
        std::string m_name;
        std::string m_id;
    };
    using VecForeignKeyContext = std::vector<ForeignKeyContext>;

    explicit Model(const std::string& type);

    explicit Model(const std::string& id, const std::string& type);

    explicit Model(const Model& other);

    using Ptr = std::unique_ptr<Model>;

    virtual ~Model();

    std::time_t get_last_modified_time() const;

    std::time_t get_creation_time() const;

    std::string get_runtime_type() const override;

    std::string get_parent_type() const;

    std::string get_parent_id() const;

    std::string get_child_id_by_type(const std::string& type) const;

    void get_child_ids_by_type(
        std::unordered_map<std::string, std::vector<std::string>>& ids);

    void get_foreign_key_fields(VecForeignKeyContext& fields) const;

    void get_foreign_key_proxy_fields(VecKeyValuePair& fields) const;

    using TypeIdsPair = std::pair<std::string, std::vector<std::string>>;
    void get_many_to_many_fields(std::vector<TypeIdsPair>& fields) const;

    void get_default_create_one_to_one_fields(VecKeyValuePair& fields) const;

    void get_one_to_one_fields(VecKeyValuePair& fields) const;

    bool has_owner() const;

    const std::string& name() const;

    void set_has_owner(bool has_owner);

    void set_name(const std::string& name);

    void set_type(const std::string& type);

    bool valid() const;

    Model& operator=(const Model& other);

  protected:
    void register_foreign_key_field(ForeignKeyField* field);
    void register_many_to_many_field(ManyToManyField* field);

    void register_one_to_one_proxy_field(DictField* field);
    void register_foreign_key_proxy_field(DictField* field);

    std::unordered_map<std::string, ForeignKeyField*> m_foreign_key_fields;
    std::unordered_map<std::string, ManyToManyField*> m_many_to_many_fields;

    std::unordered_map<std::string, DictField*> m_foreign_key_proxy_fields;
    std::unordered_map<std::string, DictField*> m_one_to_one_proxy_fields;

    bool m_has_owner{false};
    StringField m_name{"name"};
    DateTimeField m_creation_time{"creation_time"};
    DateTimeField m_last_modified_time{"last_modified_time"};

  private:
    void init();
};
using VecModelPtr = std::vector<Model::Ptr>;

class ModelCreationContext : public SerializeableWithFields {
  public:
    ModelCreationContext(const std::string& type) :
        SerializeableWithFields(type)
    {
        register_scalar_field(&m_creation_time);
        register_scalar_field(&m_last_modified_time);
    }

    void add_user(const std::string& user_id)
    {
        register_map_field(&m_created_by);
        m_created_by.set_id(user_id);
    }

    ForeignKeyField m_created_by{"created_by", "user"};
    DateTimeField m_creation_time{"creation_time"};
    DateTimeField m_last_modified_time{"last_modified_time"};
};

class ModelUpdateContext : public SerializeableWithFields {
  public:
    ModelUpdateContext(const std::string& type) : SerializeableWithFields(type)
    {
        register_scalar_field(&m_last_modified_time);
    }
    DateTimeField m_last_modified_time{"last_modified_time"};
};

class ModelFactory {
  public:
    using Ptr               = std::unique_ptr<ModelFactory>;
    virtual ~ModelFactory() = default;
    virtual std::unique_ptr<Model> create() const = 0;
    virtual std::string get_type() const          = 0;
};

template<typename T>
class TypedModelFactory : public ModelFactory {
    std::unique_ptr<Model> create() const override
    {
        return std::make_unique<T>();
    }

    std::string get_type() const override { return T::get_type(); }
};

}  // namespace hestia