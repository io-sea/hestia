#pragma once

#include "BaseField.h"
#include "Uuid.h"

#include <ctime>

namespace hestia {

/**
 * @brief A Field with a single scalar value
 *
 */
class ScalarField : public BaseField {
  public:
    ScalarField(
        const std::string& name,
        const std::string& type,
        IndexScope index_scope = IndexScope::NONE);

    virtual ~ScalarField() = default;

    const std::string& get_scalar_type() const { return m_scalar_type; }

    virtual std::string value_as_string() const = 0;

    virtual void value_from_string(const std::string& value_str) = 0;

  protected:
    std::string m_scalar_type;
};

template<typename T>
class TypedScalarField : public ScalarField {
  public:
    TypedScalarField(
        const std::string& name,
        const std::string& type,
        const T& default_value = T(),
        IndexScope index_scope = IndexScope::NONE) :
        ScalarField(name, type, index_scope), m_value(default_value)
    {
    }

    virtual ~TypedScalarField() = default;

    const T& get_value() const { return m_value; }

    void init_value(const T& value) { m_value = value; }

    void update_value(const T& value)
    {
        if (m_value != value) {
            m_value    = value;
            m_modified = true;
        }
    }

  protected:
    T m_value;
};

class UuidField : public TypedScalarField<Uuid> {
  public:
    UuidField(const std::string& name, const Uuid& default_value = {});

    std::string value_as_string() const override;

    void value_from_string(const std::string& value_str) override;
};

class StringField : public TypedScalarField<std::string> {
  public:
    StringField(const std::string& name, const std::string& default_value = {});

    std::string value_as_string() const override;

    void value_from_string(const std::string& value_str) override;
};

class DateTimeField : public TypedScalarField<std::time_t> {
  public:
    DateTimeField(const std::string& name, std::time_t default_value = 0);

    void set_as_now();

    std::string value_as_string() const override;

    void value_from_string(const std::string& value_str) override;
};

class BooleanField : public TypedScalarField<bool> {
  public:
    BooleanField(const std::string& name, bool default_value = false);

    std::string value_as_string() const override;

    void value_from_string(const std::string& value_str) override;
};

class UIntegerField : public TypedScalarField<unsigned long> {
  public:
    UIntegerField(const std::string& name, unsigned long default_value = 0);

    std::string value_as_string() const override;

    void value_from_string(const std::string& value_str) override;
};

template<typename EnumT, typename ConverterT>
class EnumField : public TypedScalarField<EnumT> {
  public:
    EnumField(const std::string& name, EnumT default_value = {}) :
        TypedScalarField<EnumT>(name, "str", default_value)
    {
        m_converter.init();
    }

    std::string value_as_string() const override
    {
        return m_converter.to_string(this->m_value);
    }

    void value_from_string(const std::string& value_str) override
    {
        this->update_value(m_converter.from_string(value_str));
    }

    ConverterT m_converter;
};


}  // namespace hestia