#pragma once

#include "RelationshipField.h"
#include "SerializeableWithFields.h"

#include <memory>
#include <stdexcept>

namespace hestia {
class Model : public SerializeableWithFields {
  public:
    Model(const std::string& type);

    Model(const std::string& id, const std::string& type);

    Model(const Model& other);

    using Ptr = std::unique_ptr<Model>;

    virtual ~Model();

    std::time_t get_last_modified_time() const;

    std::time_t get_creation_time() const;

    std::string get_runtime_type() const override;

    void get_foreign_key_fields(VecKeyValuePair& fields) const;

    void get_foreign_key_proxy_fields(VecKeyValuePair& fields) const;

    void init_creation_time(std::time_t ctime);

    void init_modification_time(std::time_t mtime);

    const std::string& name() const;

    void set_name(const std::string& name);

    void set_type(const std::string& type);

    bool valid() const;

    Model& operator=(const Model& other);

  protected:
    void register_named_foreign_key_field(NamedForeignKeyField* field);
    void register_foreign_key_proxy_field(DictField* field);

    std::unordered_map<std::string, NamedForeignKeyField*>
        m_named_foreign_key_fields;
    std::unordered_map<std::string, DictField*> m_foreign_key_proxy_fields;

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