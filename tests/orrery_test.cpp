#include "catch.hpp"

#include "orrery.hpp"

using namespace groho;

TEST_CASE("Load orrery", "[ORRERY]")
{
    Kernels kernels = { { {}, "groho-test-data/de432s.bsp" },
                        { { 809, 899 }, "groho-test-data/nep086.bsp" } };
    J2000_s begin   = GregorianDate{ 2000, 1, 1, 0 };
    J2000_s end     = GregorianDate{ 2010, 1, 1, 0 };

    Orrery::StatusCode status;
    auto bodies = load_orrery_objects(begin, end, kernels, status);

    REQUIRE(status == Orrery::StatusCode::OK);
    REQUIRE(bodies.size() == 17);
    REQUIRE(bodies[1].ephemeris->target_code == 10);
    REQUIRE(bodies[1].parent_idx == 0);
    REQUIRE(bodies[11].ephemeris->target_code == 299);
    REQUIRE(bodies[11].parent_idx == 2);
    REQUIRE(bodies[15].ephemeris->target_code == 399);
    REQUIRE(bodies[15].parent_idx == 8);

    auto orrery = Orrery(begin, end, kernels);
}
