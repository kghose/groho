/*
  The scenario object holds a parsed version of the scenario files that can


*/
#pragma once

namespace sim {
class Scenario {
public:
    Scenario(std::string fname) { ; }
    bool is_changed() { return changed; }

private:
    std::atomic<bool> changed = true;
};
}

/*
#include <istream>
#include <string>
#include <vector>

// This ugly construct is to check for file modification time
#include <sys/stat.h>
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif


namespace sim
{

const uint16_t
  ScnNoChange           = 0b000000,
  ScnLabelsChange       = 0b000001,
  ScnNeedsFullRecompute = 0b000010,
  ScnNeedsPartRecompute = 0b000100
  // More as needed
;

struct ScenarioChanges
{
  uint16_t   type_of_change;
  double     recompute_from;
};

struct Scenario
{
  std::string     main_file_name;
  std::string     name;
  std::string     description;

  std::vector<std::string>    orrery_files;
  std::vector<std::string>    flight_plan_files;

  double   start_jd;
  double   stop_jd;
  double   step_jd;
  double   dt;


  time_t scenario_last_modified;
  // There may be several files making up a scenario. After we load a
  // scenario, we set this to be the latest modification time
  // of all files loaded. The next time we check, if we find that any of the
  // files has a modification time more recent than this, we reload the
  // scenario

  ScenarioChanges scenario_changes;
  // This is filled out by the difference function and indicates what changed
  // when we reloaded the scenario

  bool         valid_scenario;
  std::string   error_message;

  Scenario() { valid_scenario = false; }
  Scenario( std::string fname );

  bool is_valid() { return valid_scenario; }
  bool requires_recompute()
  {
    return
      scenario_changes.type_of_change &
      ( ScnNeedsFullRecompute | ScnNeedsPartRecompute );
  }

  time_t fetch_scenario_modification_time();
  // Check all scenario files and give us the most recently modified time

  void verify_program_is_sorted();
  // The program must be sorted. We could easily sort it internally, but we
  // choose to be explicit about it and warn the user to fix the arrangement
  // it's possible that out of order statements in a flight plan indicates
  // a planning error

  void load();
  // Load and parse scenario file and included flight plan files

  void reload_changes();
  // If files on disk have changed, load and work out diffs to current scenario

  void load_main_file();
  // Load and parse main scenario file.

  void load_flight_plan();
  // Load and parse flight plan file.

  template<class T>
  void copy_new_params_and_diff( T& old, T _new, uint16_t type_of_change );

  std::istream& operator >> ( std::istream& is );
};


} // namespace sim
*/