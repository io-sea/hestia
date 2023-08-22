#include <catch2/catch_all.hpp>

#include "XmlUtils.h"

#include <iostream>
#include <sstream>
#include <string>

TEST_CASE("Test XmlUtils - Prolog/Tag/Test", "[common]")
{
    std::string input =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<Tag1>\n<Tag2>Text</Tag2>\n</Tag1>\n";
    std::stringstream sstr;
    hestia::XmlUtils::add_default_prolog(sstr);
    hestia::XmlUtils::open_tag(sstr, "Tag1");
    hestia::XmlUtils::add_tag_and_text(sstr, "Tag2", "Text");
    hestia::XmlUtils::close_tag(sstr, "Tag1");
    std::string output = sstr.str();

    REQUIRE(input == output);
}
