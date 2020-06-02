#include "catch.hpp"

#include "serialize.hpp"

using namespace groho;

auto sim_par = SimParams{ 0, 0, 1 };

TEST_CASE("Basic Serializer check", "[SAMPLING]")
{
    std::vector<NAIFbody> objects = { -10, 0, 2, 5 };

    auto path    = fs::temp_directory_path();
    auto sampler = Serialize(sim_par, objects, path);
    REQUIRE(fs::exists(path));
    REQUIRE(fs::exists(path / "pos-10.bin"));
    REQUIRE(fs::exists(path / "pos0.bin"));
    REQUIRE(fs::exists(path / "pos2.bin"));
    REQUIRE(fs::exists(path / "pos5.bin"));
}

TEST_CASE("Serializer vel acc check", "[SAMPLING]")
{
    std::vector<NAIFbody> objects = { 0, 2 };

    auto path    = fs::temp_directory_path();
    auto sampler = Serialize(sim_par, objects, path);

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

// TODO: Improve this test
TEST_CASE("Serializer sampling check", "[SAMPLING]")
{
    std::vector<NAIFbody> objects = { 0 };

    auto path = fs::temp_directory_path();

    {
        // We put this in a scope so that the Serialize object is deleted and
        // we flush out the last point
        auto sampler = Serialize(sim_par, objects, path);
        sampler.append({ { 1e7, 0, 0 } });
        sampler.append({ { 0, 1e7, 0 } });
        sampler.append({ { 1e7, 0, 1e7 } });
        sampler.append({ { 0, 1, 1e7 } });
    }

    std::ifstream file(path / "pos0.bin", std::ios::binary | std::ios::in);
    V3d           pos_back[4];
    file.read((char*)pos_back, sizeof(V3d) * 4);
    REQUIRE(pos_back[0] == V3d{ 0, 1e7, 0 });
    REQUIRE(pos_back[1] == V3d{ 0, 1, 1e7 });
}