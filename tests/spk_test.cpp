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
