#include <iostream>

#include "catch.hpp"

#include "state.hpp"

using namespace groho;

TEST_CASE("State vel acc check", "[SAMPLING]")
{
    auto state = DiffableState({ {}, {} }, { 0, 1 }, 1);

    state.next_pos() = { { 0, 0, 0 }, { 0, 0, 0 } };
    state.next_pos() = { { 0, 1, 0 }, { 0, 2, 0 } };
    state.next_pos() = { { 0, 4, 0 }, { 0, 4, 0 } };

    REQUIRE(state.pos()[0] == V3d{ 0, 4, 0 });
    REQUIRE(state.pos()[1] == V3d{ 0, 4, 0 });

    REQUIRE(state.vel(0) == V3d{ 0, 3, 0 });
    REQUIRE(state.vel(1) == V3d{ 0, 2, 0 });

    REQUIRE(state.acc(0) == V3d{ 0, 2, 0 });
    REQUIRE(state.acc(1) == V3d{ 0, 0, 0 });
}
