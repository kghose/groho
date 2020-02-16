#include <algorithm>

#include "catch.hpp"

#include "scenario.hpp"

TEST_CASE("Parse sample scenario", "[Scenario]")
{
    groho::Scenario scenario("example.scenario.txt");

    REQUIRE(scenario.name == "Earth to Mars");
    REQUIRE(scenario.path == "example.scenario.txt");
    REQUIRE(
        double(scenario.begin)
        == double(groho::J2000_s(groho::GregorianDate{ 2050, 1, 15, 0.5 })));

    REQUIRE(scenario.orrery_files.size() == 2);
    REQUIRE(scenario.ship_files.size() == 2);
    REQUIRE(scenario.include_set.size() == 2);
    REQUIRE(
        std::find_if(
            scenario.include_set.begin(),
            scenario.include_set.end(),
            [](const groho::NAIFParam& p) { return p.value.code == 10; })
        != scenario.include_set.end());

    REQUIRE(scenario.issues.size() == 4);
    REQUIRE(scenario.issues.at(0).line == 24);
    REQUIRE(scenario.issues.at(1).line == 39);
    REQUIRE(scenario.issues.at(2).line == 45);
    REQUIRE(scenario.issues.at(3).line == 48);
}
