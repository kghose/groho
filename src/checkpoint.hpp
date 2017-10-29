/*
  This handles storing checkpoints
*/
#pragma once

#include <vector>
#include "spaceshipstate.hpp"
#include "event.hpp"

union Checkpoint
{
  SpaceShipState state;
  Event          event;
};

typedef std::vector<Checkpoint> checkpointvec_t;