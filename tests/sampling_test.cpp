#include "catch.hpp"

#include "serialize.hpp"

using namespace groho;

TEST_CASE("Serializer", "[SAMPLING]")
{
    std::vector<NAIFbody> objects = { 0, 2, 5 };

    auto path    = fs::temp_directory_path();
    auto sampler = Serialize(1, objects, path);
    REQUIRE(fs::exists(path));
    REQUIRE(fs::exists(path / "0-pos.bin"));
    REQUIRE(fs::exists(path / "2-pos.bin"));
    REQUIRE(fs::exists(path / "5-pos.bin"));
}