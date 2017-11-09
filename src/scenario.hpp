/*
  Handles parsing scenario file and translating it into a scenario structure.
  Secretly sub-contracts out parsing of flightplans 
*/
#include <string>
#include <vector>

// This ugly construct is to check for file modification time
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif


namespace sim
{

struct Scenario
{
  std::string                   main_file_name
                                        , name
                                 , description
  ;
  std::vector<std::string>        orrery_files  
                           , flight_plan_files
  ;
  double                              start_jd
                                     , stop_jd
                                     , step_jd
  ;


  time_t scenario_last_modified;
  // There may be several files making up a scenario. After we load a 
  // scenario, we set this to be the latest modification time
  // of all files loaded. The next time we check, if we find that any of the
  // files has a modification time more recent than this, we reload the
  // scenario

  bool   scenario_has_changed;
  // Set if a reloaded scenario has any changes

  double       recompute_from;
  // This is filled out by the difference operator and is a hint to the
  // simulator that we can get away with only recomputing from the given
  // time. More sophisticated recomputation schemes are possible, we will
  // cross that bridge if we come to it.
  // The simulator is free to ignore this hint

  bool         valid_scenario;
  std::string   error_message;

  Scenario() { valid_scenario = false; }
  Scenario( std::string fname );
  
  bool is_valid() { return valid_scenario; }
  bool has_changed() { return scenario_has_changed; }
  
  time_t fetch_scenario_modification_time();
  // Check all scenario files and give us the most recently modified time

  void verify_program_is_sorted();
  // The program must be sorted. We could easily sort it internally, but we
  // choose to be explicit about it and warn the user to fix the arrangement
  // it's possible that out of order statements in a flight plan indicates
  // a planning error  


  Scenario& operator -( const Scenario &rhs );
  // fill in the recompute_from field based on what differences there are
  // between the two scenarios

  bool operator !=( const Scenario &rhs ) const;
  bool operator ==( const Scenario &rhs ) const;
  
  void load();
  // Load and parse scenario file and included flight plan files

  bool load_main_file();
  // Load and parse main scenario file. Return true if there are changes

  bool load_flight_plan();
  // Load and parse flight plan file. Return true if there are changes
  
};

} // namespace sim