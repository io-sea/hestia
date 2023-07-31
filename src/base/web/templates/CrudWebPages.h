#pragma once

#include <string>

namespace hestia {
class CrudWebPages {
  public:
    static std::string get_item_view(
        const std::string& type, const std::string& item_json)
    {
        std::string body =
            "<html><script>"
            "function parseJson(){"
            "var element = document.getElementById(\"json\");"
            "var obj = JSON.parse(element.innerText);"
            "element.innerHTML = JSON.stringify(obj, undefined, 2);}"
            "</script>"
            "<body onload=\"parseJson()\">";

        body += "<h1>" + type + "s</h1><pre id=\"json\">";
        body += item_json;
        body += "</pre></body></html>";
        return body;
    }
};
}  // namespace hestia