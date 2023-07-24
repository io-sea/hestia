#pragma once

#include "StringUtils.h"

#include <algorithm>
#include <stdexcept>

#define STRINGABLE_ENUM(TYPE, ...)                                             \
    enum class TYPE { __VA_ARGS__ };                                           \
    struct TYPE##_enum_string_converter {                                      \
        TYPE##_enum_string_converter& init()                                   \
        {                                                                      \
            std::string values(#__VA_ARGS__);                                  \
            std::vector<std::string> upper_vals;                               \
            StringUtils::split(values, ',', upper_vals);                       \
            for (const auto& value : upper_vals) {                             \
                auto lower = StringUtils::to_lower(value);                     \
                StringUtils::trim(lower);                                      \
                m_values.push_back(lower);                                     \
            }                                                                  \
            return *this;                                                      \
        }                                                                      \
                                                                               \
        std::string to_string(TYPE value) const                                \
        {                                                                      \
            return m_values[static_cast<int>(value)];                          \
        }                                                                      \
                                                                               \
        TYPE from_string(const std::string& input) const                       \
        {                                                                      \
            const auto iter =                                                  \
                std::find(m_values.begin(), m_values.end(), input);            \
            if (iter != m_values.end()) {                                      \
                return static_cast<TYPE>(iter - m_values.begin());             \
            }                                                                  \
            throw std::runtime_error("Bad input string");                      \
        }                                                                      \
        std::vector<std::string> m_values;                                     \
    };
