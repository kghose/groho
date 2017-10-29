/*
  Class we use to flag events with
*/
#pragma once

namespace sim
{

enum class EventType { 
  Launch, 
  Landing,
  Crash,
  Encounter,
  BingoFuel
};

class Event
{
public:
  EventType      event_type;
  double        julian_date;
  std::string          name;
  std::string       remarks;

  Event( 
      EventType et, double jd, std::string name, std::string remarks )
      : 
      event_type( et ), julian_date( jd ), name( name ), remarks( remarks )
  {}
};

} // namespace sim
