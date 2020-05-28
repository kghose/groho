#include "catch.hpp"

#include "orrery.hpp"

using namespace groho;

TEST_CASE("Load orrery", "[ORRERY]")
{
    KernelTokens kernels = { { {}, "groho-test-data/de432s.bsp" },
                             { { 809, 899 }, "groho-test-data/nep086.bsp" } };
    J2000_s      begin   = GregorianDate{ 2000, 1, 1, 0 };
    J2000_s      end     = GregorianDate{ 2010, 1, 1, 0 };

    Orrery::StatusCode status;
    auto bodies = load_orrery_objects(begin, end, kernels, status);

    REQUIRE(status == Orrery::StatusCode::OK);
    REQUIRE(bodies.size() == 17);

    for (size_t i = 1; i < bodies.size(); i++) {
        REQUIRE(bodies[i].parent_idx < i); // The ordering property we need
    }

    for (size_t i = 1; i < bodies.size(); i++) {
        if (bodies[i].ephemeris->target_code == 299) {
            size_t venus_bc = bodies[i].parent_idx;
            REQUIRE(bodies[venus_bc].ephemeris->target_code == 2);
            REQUIRE(bodies[venus_bc].parent_idx == 0);
            break;
        }
    }

    auto orrery = Orrery(begin, end, kernels);
    // auto body_codes = orrery.list_bodies();

    // REQUIRE(body_codes.size() == 17);
}

TEST_CASE("Missing kernel file", "[ORRERY]")
{
    KernelTokens kernels
        = { { { 909, 999 }, "groho-test-data/non-existent.bsp" } };
    J2000_s begin = GregorianDate{ 2000, 1, 1, 0 };
    J2000_s end   = GregorianDate{ 2010, 1, 1, 0 };

    auto orrery = Orrery(begin, end, kernels);
    REQUIRE(orrery.status() == Orrery::StatusCode::WARNING);
}

TEST_CASE("Load orrery with date problems", "[ORRERY]")
{
    KernelTokens kernels = { { {}, "groho-test-data/de432s.bsp" },
                             { { 809, 899 }, "groho-test-data/nep086.bsp" } };
    J2000_s      begin   = GregorianDate{ 1000, 1, 1, 0 };
    J2000_s      end     = GregorianDate{ 1010, 1, 1, 0 };

    auto orrery = Orrery(begin, end, kernels);
    REQUIRE(orrery.status() == Orrery::StatusCode::ERROR);
}

TEST_CASE("Load orrery with center problems", "[ORRERY]")
{
    KernelTokens kernels = { { { 809, 899 }, "groho-test-data/nep086.bsp" } };
    J2000_s      begin   = GregorianDate{ 2000, 1, 1, 0 };
    J2000_s      end     = GregorianDate{ 2010, 1, 1, 0 };

    auto orrery = Orrery(begin, end, kernels);
    REQUIRE(orrery.status() == Orrery::StatusCode::ERROR);
}

TEST_CASE("Load orrery with missing bodies", "[ORRERY]")
{
    KernelTokens kernels = { { { 909, 999 }, "groho-test-data/nep086.bsp" } };
    J2000_s      begin   = GregorianDate{ 2000, 1, 1, 0 };
    J2000_s      end     = GregorianDate{ 2010, 1, 1, 0 };

    auto orrery = Orrery(begin, end, kernels);
    REQUIRE(orrery.status() == Orrery::StatusCode::WARNING);
}
