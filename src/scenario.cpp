#include <fstream>
#include <unordered_map>

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
  time_t tmod = fetch_scenario_modification_time();
  if( tmod > scenario_last_modified )
  {
    DLOG_S(INFO) << "Reloading " << main_file_name;    
    
    // do the work of loading the files here 
    load_main_file();

    valid_scenario = true;
    scenario_has_changed = true;    
    scenario_last_modified = tmod;
  }
  else
  {
    scenario_has_changed = false;
  }
}

bool 
Scenario::load_main_file()
{
  std::ifstream main_file( main_file_name );
  TokenizedScenario ts;
  main_file >> ts;

  name        = ts.get_header_value( "name",        "No Name"        )[0];
  description = ts.get_header_value( "description", "No description" )[0];

  start_jd = std::stod( ts.get_header_value( "start", "2458066.5" )[0] );
  stop_jd  = std::stod( ts.get_header_value( "stop",  "2458061.5" )[0] );
  step_jd  = std::stod( ts.get_header_value( "step",  "0.00001"   )[0] );
  
  orrery_files      = ts.get_header_value( "orrery", "",     Expecting::ZeroOrMore );
  flight_plan_files = ts.get_header_value( "flightplan", "", Expecting::ZeroOrMore );

  return true;
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


Scenario& 
Scenario::operator -( const Scenario &rhs )
{
  *this = rhs;
  recompute_from = start_jd;
  // For now, we simply recompute everything
  scenario_has_changed = false;
  return *this;
}


void verify_program_is_sorted() {}


} // namespace sim