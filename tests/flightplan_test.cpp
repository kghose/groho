#include "catch.hpp"

#include "flightplan.hpp"

TEST_CASE("Parse sample flight plan", "[FlightPlan]")
{
    groho::FlightPlan flight_plan
        = groho::load_flight_plan("flightplan/plan1.groho.txt");

    REQUIRE(flight_plan.name == "Ma Durga");
    REQUIRE(flight_plan.path == "flightplan/plan1.groho.txt");
    REQUIRE(double(flight_plan.initial_condition.t) == 0);

    REQUIRE(flight_plan.route.size() == 3);
    REQUIRE(
        flight_plan.route[0].t
        == groho::J2000_s(groho::GregorianDate{ 2050, 01, 01, 0.5 }));

    REQUIRE(flight_plan.route[2].command == "orbit");
    REQUIRE(flight_plan.route[2].arguments[1] == "200x200");

    REQUIRE(flight_plan.errors.size() == 2);
    REQUIRE(flight_plan.errors.at(0).line == 3);
    REQUIRE(flight_plan.errors.at(1).line == 12);
}
