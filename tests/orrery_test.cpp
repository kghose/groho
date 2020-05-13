#include "catch.hpp"

#include "orrery.hpp"

using namespace groho;

TEST_CASE("Load orrery", "[ORRERY]")
{
    std::vector<NAIFbody>    codes = { 399, 812, 301 };
    std::vector<std::string> file_names
        = { "groho-test-data/de432s.bsp", "groho-test-data/nep086.bsp" };

    J2000_s begin = GregorianDate{ 2000, 1, 1, 0 };
    J2000_s end   = GregorianDate{ 2010, 1, 1, 0 };

    auto orrery = Orrery::load(codes, file_names, begin, end);
}