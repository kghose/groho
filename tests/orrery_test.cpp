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
    auto objects = load_orrery_objects(begin, end, kernels, status);

    REQUIRE(status == Orrery::StatusCode::OK);
    REQUIRE(objects.size() == 17);

    for (size_t i = 1; i < objects.size(); i++) {
        REQUIRE(objects[i].parent_idx < i); // The ordering property we need
    }

    for (size_t i = 1; i < objects.size(); i++) {
        if (objects[i].ephemeris->target_code == 299) {
            size_t venus_bc = objects[i].parent_idx;
            REQUIRE(objects[venus_bc].ephemeris->target_code == 2);
            REQUIRE(objects[venus_bc].parent_idx == 0);
            break;
        }
    }

    auto orrery = Orrery(begin, end, kernels);

    REQUIRE(orrery.get_bodies().size() == 16);
    REQUIRE(orrery.get_grav_body_idx().size() == 12);
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
