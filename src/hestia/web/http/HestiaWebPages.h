#pragma once

#include "HsmItem.h"
#include "HsmNode.h"
#include "User.h"
#include <string>

namespace hestia {
class HestiaWebPages {
  public:
    static std::string get_default_index_view()
    {
        std::string body = "<html><body>";
        body += "<p><a href=\"" + get_url(HsmItem::hsm_object_name) + "\">"
                + get_url(HsmItem::hsm_object_name) + "</a></p>";
        body += "<p><a href=\"" + get_url(HsmItem::dataset_name) + "\">"
                + get_url(HsmItem::dataset_name) + "</a></p>";
        body += "<p><a href=\"" + get_url(HsmItem::tier_name) + "\">"
                + get_url(HsmItem::tier_name) + "</a></p>";
        body += "<p><a href=\"" + get_url(HsmItem::object_store_backend_name)
                + "\">" + get_url(HsmItem::object_store_backend_name)
                + "</a></p>";
        body += "<p><a href=\"" + get_url(User::get_type()) + "\">"
                + get_url(User::get_type()) + "</a></p>";
        body += "<p><a href=\"" + get_url(HsmNode::get_type()) + "\">"
                + get_url(HsmNode::get_type()) + "</a></p>";
        body += "</body></html>";
        return body;
    }

    static std::string get_url(const std::string& type)
    {
        return "api/v1/" + type + "s";
    }
};
}  // namespace hestia