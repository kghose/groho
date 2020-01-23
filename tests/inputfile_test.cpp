#include "catch.hpp"

#include "inputfile.hpp"

using namespace groho;

TEST_CASE("Missing input file parsing", "[InputFile]")
{
    groho::InputFile in("non-existent.txt");
    auto             lines = in.load();
}

TEST_CASE("Input file parsing", "[InputFile]")
{
    groho::InputFile in("test-input.txt");
    auto             lines = in.load();

    REQUIRE(lines.at(0).line == 9);
    REQUIRE(lines.at(0).key == "key1");
    REQUIRE(lines.at(0).value == "value1");

    REQUIRE(lines.at(1).line == 10);
    REQUIRE(lines.at(1).key == "key2");
    REQUIRE(lines.at(1).value == "2050.01.15:0.5");

    REQUIRE(lines.at(3).line == 14);
    REQUIRE(lines.at(3).key == "key4");
    REQUIRE(lines.at(3).value == "this is tight spacing");

    REQUIRE(lines.at(4).line == 16);
    REQUIRE(lines.at(4).key == "key5");
    REQUIRE(lines.at(4).value == "");
}
