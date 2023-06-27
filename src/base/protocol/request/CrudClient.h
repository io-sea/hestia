#pragma once

#include "Metadata.h"
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

    virtual bool exists(const Uuid& id) const = 0;

    virtual bool exists(const std::string& name) const = 0;

    virtual void get(ItemT& item) const = 0;

    virtual void list(const Metadata& query, std::vector<Uuid>& ids) const = 0;

    virtual void multi_get(
        const Metadata& query, std::vector<ItemT>& items) const = 0;

    virtual void put(
        const ItemT& item, bool generate_id, ItemT& updated_item) const = 0;

    virtual void remove(const Uuid& id) const = 0;

  protected:
    virtual Uuid generate_id(const ItemT& item) const = 0;

    bool matches_query(const ItemT& item, const Metadata& query) const
    {
        return m_adapter->matches_query(item, query);
    }

    void to_string(
        const ItemT& item, std::string& output, const Uuid& id = {}) const
    {
        m_adapter->to_string(item, output, id);
    }

    void from_string(const std::string& input, ItemT& item) const
    {
        m_adapter->from_string(input, item);
    }

    void from_string(const std::string& input, std::vector<ItemT>& items) const
    {
        m_adapter->from_string(input, items);
    }

  private:
    std::unique_ptr<StringAdapter<ItemT>> m_adapter;
};
}  // namespace hestia