#include "catch.hpp"

#include "orrery.hpp"

using namespace groho;

TEST_CASE("Load orrery", "[ORRERY]")
{
    Kernels kernels = { { {}, "groho-test-data/de432s.bsp" },
                        { { 809, 899 }, "groho-test-data/nep086.bsp" } };
    J2000_s begin   = GregorianDate{ 2000, 1, 1, 0 };
    J2000_s end     = GregorianDate{ 2010, 1, 1, 0 };
    auto    orrery  = Orrery::load(begin, end, kernels);
}