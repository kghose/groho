#include "catch.hpp"

#include "scenario.hpp"

TEST_CASE("Parse sample scenario", "[Scenario]")
{
    groho::Scenario scenario = groho::load_scenario("example.scenario.txt");

    REQUIRE(scenario.name == "Earth to Mars");
    REQUIRE(
        double(scenario.begin)
        == double(groho::J2000_s(groho::GregorianDate{ 2050, 1, 15, 0.5 })));

    REQUIRE(scenario.orrery_files.size() == 2);
    REQUIRE(scenario.ship_files.size() == 2);
    REQUIRE(scenario.include_set.size() == 2);
    REQUIRE(
        scenario.include_set.find(groho::NAIFbody(10))
        != scenario.include_set.end());

    REQUIRE(scenario.errors.size() == 2);
    REQUIRE(scenario.errors.at(0).line == 24);
    REQUIRE(scenario.errors.at(1).line == 39);
}
