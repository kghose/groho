/*
  Class we use to flag events with
*/
#pragma once

//#include "checkpoint.hpp"

namespace sim
{

enum class EventType { 
  Launch, 
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

  Event( double jd ): jd( jd ) {}

  Event( double jd, EventType et, std::string name, std::string remarks ) : 
      jd( jd ), name( name ), remarks( remarks ), event_type( et ) 
  {}  
};

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
