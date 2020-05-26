#include <algorithm>

#include "catch.hpp"

#include "scenario.hpp"

using namespace groho;

TEST_CASE("Parse sample scenario", "[SCENARIO]")
{
    auto     lines = load_input_file("../examples/001.basics/scn.groho.txt");
    Scenario scenario(*lines);

    REQUIRE(scenario.begin == J2000_s(GregorianDate{ 2050, 01, 01, 0.5 }));

    REQUIRE(scenario.kernels.size() == 2);
    REQUIRE(scenario.kernels[0].path == "de432s.bsp");
    REQUIRE(scenario.kernels[1].path == "nep086.bsp");
    REQUIRE(scenario.kernels[1].codes.count(NAIFbody(899)) == 1);

    /*
        REQUIRE(scenario.name == "Earth to Mars");
        REQUIRE(scenario.path == "example.scenario.txt");
        REQUIRE(
            double(scenario.begin)
            == double(groho::J2000_s(groho::GregorianDate{ 2050, 1, 15, 0.5
       })));

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
    */
}
