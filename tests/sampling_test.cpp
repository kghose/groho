#include "catch.hpp"

#include "serialize.hpp"

using namespace groho;

TEST_CASE("Basic Serializer check", "[SAMPLING]")
{
    std::vector<NAIFbody> objects = { 0, 2, 5 };

    auto path    = fs::temp_directory_path();
    auto sampler = Serialize(1, objects, path);
    REQUIRE(fs::exists(path));
    REQUIRE(fs::exists(path / "0-pos.bin"));
    REQUIRE(fs::exists(path / "2-pos.bin"));
    REQUIRE(fs::exists(path / "5-pos.bin"));
}

TEST_CASE("Serializer vel acc check", "[SAMPLING]")
{
    std::vector<NAIFbody> objects = { 0, 2 };

    auto path    = fs::temp_directory_path();
    auto sampler = Serialize(1, objects, path);

    sampler.append({ { 0, 0, 0 }, { 0, 0, 0 } });
    sampler.append({ { 0, 1, 0 }, { 0, 2, 0 } });
    sampler.append({ { 0, 4, 0 }, { 0, 4, 0 } });

    REQUIRE(sampler.pos(0) == V3d{ 0, 4, 0 });
    REQUIRE(sampler.pos(1) == V3d{ 0, 4, 0 });

    REQUIRE(sampler.vel(0) == V3d{ 0, 3, 0 });
    REQUIRE(sampler.vel(1) == V3d{ 0, 2, 0 });

    REQUIRE(sampler.acc(0) == V3d{ 0, 2, 0 });
    REQUIRE(sampler.acc(1) == V3d{ 0, 0, 0 });
}