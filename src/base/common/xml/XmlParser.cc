#include "XmlParser.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iostream>
#include <stdexcept>

namespace hestia {

void convert_layer(XmlElement& working_element, xmlNode* working_node)
{
    auto child = working_node->children;
    while (child != nullptr) {
        if (child->type == xmlElementType::XML_TEXT_NODE) {
            if (child->content != nullptr) {
                working_element.set_text(
                    reinterpret_cast<const char*>(child->content));
            }
        }
        else if (child->type == xmlElementType::XML_ELEMENT_NODE) {
            std::string name;
            if (child->name != nullptr) {
                name = reinterpret_cast<const char*>(child->name);
            }
            auto child_element = XmlElement::create(name);
            convert_layer(*child_element, child);
            working_element.add_child(std::move(child_element));
        }

        child = child->next;
    }
}

XmlDocument::Ptr XmlParser::run(const std::string& content) const
{
    auto ret_doc = XmlDocument::create();

    auto xml_doc =
        xmlReadMemory(content.data(), content.length(), "", nullptr, 0);
    if (xml_doc == nullptr) {
        auto p_error = xmlGetLastError();
        throw std::runtime_error(
            "Failed to parse xml with: " + std::string(p_error->message));
    }

    auto root = xml_doc->children;
    if (root != nullptr && root->type == xmlElementType::XML_ELEMENT_NODE) {
        std::string name;
        if (root->name != nullptr) {
            name = reinterpret_cast<const char*>(root->name);
        }
        auto root_element = XmlElement::create(name);
        convert_layer(*root_element, root);
        ret_doc->set_root(std::move(root_element));
    }
    xmlFreeDoc(xml_doc);

    xmlCleanupParser();

    return ret_doc;
}
}  // namespace hestia
