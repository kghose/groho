/*
  Class we use to flag events with
*/
#pragma once

#include "checkpoint.hpp"

namespace sim
{

enum class EventType { 
  Launch, 
  Landing,
  Crash,
  Encounter,
  BingoFuel
};

struct Event : public Checkpoint
{
  EventType      event_type;
  std::string          name;
  std::string       remarks;

  Event( double jd ): Checkpoint( jd ) {}

  Event( 
      double jd, EventType et, std::string name, std::string remarks )
      : 
      Checkpoint( jd ), event_type( et ), name( name ), remarks( remarks ) 
  {}
};

} // namespace sim
