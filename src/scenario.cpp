#include <fstream>
#include <unordered_map>
#include <set>

#include "tokenizedscenario.hpp"
#include "scenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

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


} // namespace sim