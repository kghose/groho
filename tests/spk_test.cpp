#include "catch.hpp"

#include "spk.hpp"

using namespace groho;

TEST_CASE("A bad file should not load", "[SPK]")
{
    auto _spk = SpkFile::load("groho-test-data/de");
    REQUIRE(!_spk);
}

TEST_CASE("Load a regular BSP file", "[SPK]")
{
    auto _spk = SpkFile::load("groho-test-data/de432s.bsp");
    REQUIRE(_spk);

    auto spk = *_spk;
    REQUIRE(spk.summaries.size() == 14);

    const auto& earth = spk.summaries[NAIFbody(399)];
    REQUIRE(earth.begin_second == J2000_s(GregorianDate{ 1949, 12, 14, 0 }));
    REQUIRE(earth.center_id == 3);
}

TEST_CASE("Load an ephemeris from BSP file", "[SPK]")
{
    auto _spk = SpkFile::load("groho-test-data/de432s.bsp");
    auto spk  = *_spk;

    auto eph1 = spk.load_ephemeris(
        NAIFbody(301),
        J2000_s(GregorianDate{ 2000, 01, 01 }),
        J2000_s(GregorianDate{ 2010, 01, 01 }));
    REQUIRE(eph1);

    // Non existent body
    auto eph2 = spk.load_ephemeris(
        NAIFbody(302),
        J2000_s(GregorianDate{ 2000, 01, 01 }),
        J2000_s(GregorianDate{ 2010, 01, 01 }));
    REQUIRE(!eph2);

    // Out of range
    auto eph3 = spk.load_ephemeris(
        NAIFbody(301),
        J2000_s(GregorianDate{ 2100, 01, 01 }),
        J2000_s(GregorianDate{ 2110, 01, 01 }));
    REQUIRE(!eph3);
}
