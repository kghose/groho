#include <fstream>
#include <iostream>
#include <set>

#include "scenario.hpp"
//#include "tokenizedscenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

const std::string wspace = " \t\n\r\f\v";

std::string trim_whitespace(std::string line)
{
    line.erase(0, line.find_first_not_of(wspace));
    line.erase(line.find_last_not_of(wspace) + 1);
    return line;
}

std::string trim_comments(std::string line)
{
    return line.erase(std::min(line.find_first_of(';'), line.size()));
}

std::optional<KeyValue> get_key_value(std::string line)
{
    KeyValue kv;
    size_t   n0 = line.find_first_of('=');

    if (n0 > line.size()) {
        return {};
    }

    kv.key   = trim_whitespace(line.substr(0, n0 - 1));
    kv.value = trim_whitespace(line.substr(n0 + 1));
    return kv;
}

std::optional<Configuration> parse_configuration(std::string fname)
{

    std::ifstream cfile(fname);
    int           line_no = 0;
    std::string   line;
    Configuration cfg;

    LOG_S(INFO) << "Parsing scenario file " << fname;

    while (std::getline(cfile, line)) {
        line_no++;

        line = trim_whitespace(trim_comments(line));

        if (line.length() == 0) // Ignore empty lines
            continue;

        std::optional<KeyValue> kv = get_key_value(line);
        if (kv) {
            if (!cfg.set_key_value(kv)) {
                LOG_S(WARNING) << "Line " << line_no << ": Unknown key "
                               << kv->key << ": ignoring";
            }
        } else {
            LOG_S(ERROR) << "Error parsing line: " << line_no << " (" << line
                         << ")";
        }
    }
    return cfg;
}

bool Scenario::is_changed()
{
    std::optional<Configuration> new_config = parse_configuration(fname);

    changed = false;
    return changed;
}

/*
const double sec_per_jd = 86400;


time_t
file_last_modified_time( std::string fname )
{
  struct stat result;
  if( stat( fname.c_str(), &result ) == 0 )
  {
    return result.st_mtimespec.tv_sec;
  }
  else
  {
    LOG_S(ERROR) << "Could not stat " << fname;
    return 0;  // This is our error value
  }
}


Scenario::Scenario( std::string fname )
{
  main_file_name = fname;
  load();
}

void
Scenario::load()
{
  DLOG_S(INFO) << "Loading " << main_file_name;
  time_t tmod = fetch_scenario_modification_time();

  // do the work of loading the files here
  valid_scenario = true;
  load_main_file();


  scenario_last_modified = tmod;
}

void
Scenario::reload_changes()
{
  time_t tmod = fetch_scenario_modification_time();
  if( tmod > scenario_last_modified )
  {
    load();
  }
  else
  {
    scenario_changes.type_of_change = ScnNoChange;
  }
}

template<class T>
bool different( const T a, const T b )
{
  return( a != b );
}

template<class T>
bool different( const std::vector<T> a, const std::vector<T> b )
{
  std::set<std::string>
    old_set( a.begin(), a.end() ),
    new_set( b.begin(), b.end() );
  return( a != b );
}


template<class T>
void
Scenario::copy_new_params_and_diff(
  T& old, T _new, uint16_t type_of_change )
{
  if( different( old, _new ) ) {
    old = _new;
    scenario_changes.type_of_change |= type_of_change;
  }
}


void
Scenario::load_main_file()
{
  std::ifstream main_file( main_file_name );
  TokenizedScenario ts;
  main_file >> ts;

  copy_new_params_and_diff(
    name,
    ts.get_parameter( "name", "No Name" ),
    ScnLabelsChange
  );

  copy_new_params_and_diff(
    description,
    ts.get_parameter( "description", "No description" ),
    ScnLabelsChange
  );

  copy_new_params_and_diff(
    step_jd,
    std::stod( ts.get_parameter( "step",  "0.00001"   ) ),
    ScnNeedsFullRecompute
  );
  dt = step_jd * sec_per_jd;
  dt2 = dt * dt;

  copy_new_params_and_diff(
    start_jd,
    std::stod( ts.get_parameter( "start", "2458066.5" ) ),
    ScnNeedsFullRecompute
  );

  copy_new_params_and_diff(
    stop_jd,
    std::stod( ts.get_parameter( "stop",  "2458061.5" ) ),
    ScnNeedsPartRecompute
  );

  copy_new_params_and_diff(
    orrery_files,
    ts.get_parameter_list( "orrery", "", Expecting::ZeroOrMore ),
    ScnNeedsFullRecompute
  );

  copy_new_params_and_diff(
    flight_plan_files,
    ts.get_parameter_list( "flightplan", "", Expecting::ZeroOrMore ),
    ScnNeedsFullRecompute
  );
}

time_t
Scenario::fetch_scenario_modification_time()
{
  time_t t = file_last_modified_time( main_file_name );
  for( auto& fname : flight_plan_files )
  {
    t = std::max( t, file_last_modified_time( fname ) );
  }
  return t;
}

void verify_program_is_sorted() {}


std::istream& operator Scenario::>> ( std::istream& is )
{

}

*/

} // namespace sim