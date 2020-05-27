#include <algorithm>

#include "catch.hpp"

#include "scenario.hpp"

using namespace groho;

TEST_CASE("Scenario preamble", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario;
    scenario.parse_preamble(*lines);

    REQUIRE(scenario.begin == J2000_s(GregorianDate{ 2050, 01, 01, 0.5 }));
}

TEST_CASE("Scenario kernels", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario;
    scenario.parse_kernels(*lines);

    REQUIRE(scenario.kernel_tokens.size() == 2);
    REQUIRE(scenario.kernel_tokens[0].path == "de432s.bsp");
    REQUIRE(scenario.kernel_tokens[1].path == "nep086.bsp");
    REQUIRE(scenario.kernel_tokens[1].codes.count(NAIFbody(899)) == 1);
}

TEST_CASE("Scenario craft commands", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario;
    scenario.parse_plans(*lines);

    REQUIRE(scenario.spacecraft_tokens.size() == 2);
}

TEST_CASE("Scenario integrative test", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario(*lines);
}
