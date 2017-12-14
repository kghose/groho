/*
  Class we use to flag events with
*/
#pragma once

#include <string>
#include <iostream>


namespace sim
{

enum class EventType { 
  Launch = 0, 
  Landing,
  Crash,
  Encounter,
  BingoFuel
};


struct Event
{
  float                  jd;
  std::string          name;
  std::string       remarks;
  EventType      event_type;

  Event() {}
  Event( double jd ): jd( jd ) {}
  Event( double jd, std::string name, EventType et ) : 
      jd( jd ), name( name ), event_type( et ) {}
  Event( double jd, std::string name, EventType et, std::string remarks ) : 
      jd( jd ), name( name ), remarks( remarks ), event_type( et ) {}

  std::string event_type_str() const
  {
    switch( event_type ) {
      case EventType::Launch: return "Launch";
      case EventType::Landing: return "Landing";
      case EventType::Crash: return "Crash";
      case EventType::Encounter: return "Encounter";
      case EventType::BingoFuel: return "BingoFuel";
      default: return "Unknown";
    }
  }
};

inline
std::ostream& 
operator << ( std::ostream& os, const Event& e )
{
  os << e.jd << ", " << e.name << ", " << e.event_type_str() << ", " << e.remarks;
  return os;
}


inline bool operator == ( const Event& e, const float _jd ) { return e.jd == _jd; }
inline bool operator == ( const float _jd, const Event& e ) { return e.jd == _jd; }
inline bool operator != ( const Event& e, const float _jd ) { return e.jd != _jd; }
inline bool operator != ( const float _jd, const Event& e ) { return e.jd != _jd; }
inline bool operator <  ( const Event& e, const float _jd ) { return e.jd <  _jd; }
inline bool operator <= ( const Event& e, const float _jd ) { return e.jd <= _jd; }
inline bool operator >  ( const Event& e, const float _jd ) { return e.jd >  _jd; }
inline bool operator >= ( const Event& e, const float _jd ) { return e.jd >= _jd; }
inline bool operator <  ( const float _jd, const Event& e ) { return  _jd <  e.jd; }
inline bool operator <= ( const float _jd, const Event& e ) { return  _jd <= e.jd; }
inline bool operator >  ( const float _jd, const Event& e ) { return  _jd >  e.jd; }
inline bool operator >= ( const float _jd, const Event& e ) { return  _jd >= e.jd; }

} // namespace sim
