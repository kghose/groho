#include <algorithm>

#include "catch.hpp"

#include "scenario.hpp"

using namespace groho;

TEST_CASE("Scenario preamble", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario;
    scenario.parse_preamble(*lines);

    REQUIRE(scenario.sim.begin == J2000_s(GregorianDate{ 2020, 01, 01, 0.5 }));
}

TEST_CASE("Scenario kernels", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario;
    scenario.parse_kernels(*lines);

    REQUIRE(scenario.kernel_tokens.size() == 2);
    REQUIRE(scenario.kernel_tokens[0].path.filename() == "de432s.bsp");
    REQUIRE(fs::exists(scenario.kernel_tokens[0].path));
    REQUIRE(scenario.kernel_tokens[1].path.filename() == "nep086.bsp");
    REQUIRE(fs::exists(scenario.kernel_tokens[1].path));
    REQUIRE(scenario.kernel_tokens[1].codes.count(NAIFbody(899)) == 1);
}

TEST_CASE("Scenario craft commands", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario;
    scenario.parse_plans(*lines);

    REQUIRE(scenario.spacecraft_tokens.size() == 2);
    REQUIRE(scenario.spacecraft_tokens[1].code == -2000);
    REQUIRE(scenario.spacecraft_tokens[1].initial_condition.params[0] == "301");
}

TEST_CASE("Scenario integrative test", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario(*lines);
}
