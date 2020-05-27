#include "catch.hpp"

#include "inputfile.hpp"

using namespace groho;

TEST_CASE("Missing input file parsing", "[InputFile]")
{
    auto lines = load_input_file("non-existent.txt");
    REQUIRE(!lines);
}

TEST_CASE("Input file parsing", "[InputFile]")
{
    auto lines = load_input_file("../examples/001.basics/scn.groho.txt");

    REQUIRE((*lines).size() == 17);

    REQUIRE((*lines)[0].key == "start");
    REQUIRE((*lines)[7].key == "2050.01.01:0.5");
    REQUIRE((*lines)[8].key == "plan");
    REQUIRE((*lines)[10].key == "2050.01.02:0.5");
    REQUIRE((*lines)[11].key == "2050.01.03:0.5");
    REQUIRE((*lines)[12].key == "2050.01.04:0.5");
    REQUIRE((*lines)[14].status.code == ParseStatus::ERROR);
}

TEST_CASE("String split on white space", "[StringSplit]")
{
    auto split = split_string(" The quick     brown fox   ");

    REQUIRE(split.at(0) == "The");
    REQUIRE(split.at(3) == "fox");
    REQUIRE(split.size() == 4);
}

TEST_CASE("String split on comma", "[StringSplit]")
{
    auto split = split_string(
        "Mercury BC,  2.2031780000000021E+04,            0", ",");

    REQUIRE(split.at(0) == "Mercury BC");
    REQUIRE(split.at(1) == "2.2031780000000021E+04");
    REQUIRE(split.at(2) == "0");
}
