/*
  The scenario object holds a parsed version of the scenario files that can


*/
#pragma once

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

namespace sim {

struct KeyValue {
    std::string key, value;
};

struct Configuration {

    std::string name     = "No name";
    double      begin_jd = 2458248.5;
    double      end_jd   = 2458249.5;
    double      step     = 60;

    std::set<std::string> orrery;
    std::set<std::string> flightplan;

    typedef std::string sst;

    bool set_key_value(std::optional<KeyValue> kv)
    {
        static std::unordered_map<sst, std::function<void(sst)>> keyword_map{

            { "name", [=](sst v) { name      = v; } },
            { "begin", [=](sst v) { begin_jd = stof(v); } },
            { "end", [=](sst v) { end_jd     = stof(v); } },
            { "step", [=](sst v) { step      = stof(v); } },
            { "orrery", [=](sst v) { orrery.insert(v); } },
            { "flightplan", [=](sst v) { flightplan.insert(v); } }

        };

        if (keyword_map.count(kv->key)) {
            keyword_map[kv->key](kv->value);
            return true;
        } else {
            return false;
        }
    };
};

class Scenario {
public:
    Scenario(std::string fname_) { fname = fname_; }
    bool is_valid() { return valid; }
    bool is_changed();
    void change_noted() { changed = false; }

private:
    std::string fname;

    std::atomic<bool> changed = true, valid = false;
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