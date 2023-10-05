#include <catch2/catch_all.hpp>

#include "XmlParser.h"

#include <iostream>
#include <sstream>
#include <string>

TEST_CASE("Test XmlParser", "[common]")
{
    std::string input =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<Tag1>\n<Tag2>Text</Tag2>\n</Tag1>\n";

    hestia::XmlParser parser;
    auto doc = parser.run(input);

    REQUIRE(doc->get_root() != nullptr);
    REQUIRE(doc->get_root()->get_tag_name() == "Tag1");
    REQUIRE(doc->get_root()->get_children().size() == 1);
    REQUIRE(doc->get_root()->get_children()[0]->get_tag_name() == "Tag2");
    REQUIRE(doc->get_root()->get_children()[0]->get_text() == "Text");
}