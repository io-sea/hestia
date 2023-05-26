#pragma once

#include "StringAdapter.h"

#include <memory>
#include <string>
#include <vector>

namespace hestia {

template<typename ItemT>
class CrudClient {
  public:
    CrudClient(std::unique_ptr<StringAdapter<ItemT>> adapter) :
        m_adapter(std::move(adapter))
    {
    }

    virtual ~CrudClient() = default;

    virtual void get(ItemT& item) const = 0;

    virtual bool exists(const ItemT& item) const = 0;

    virtual void list(std::vector<std::string>& ids) const = 0;

    virtual void multi_get(std::vector<ItemT>& items) const = 0;

    virtual void put(const ItemT& item) const = 0;

    virtual void remove(const ItemT& item) const = 0;

  protected:
    void to_string(const ItemT& item, std::string& output) const
    {
        m_adapter->to_string(item, output);
    }

    void from_string(const std::string& input, ItemT& item) const
    {
        m_adapter->from_string(input, item);
    }

  private:
    std::unique_ptr<StringAdapter<ItemT>> m_adapter;
};
}  // namespace hestia