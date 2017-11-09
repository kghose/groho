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
    DLOG_S(ERROR) << "Could not stat " << fname;    
    return 0;  // This is our error value
  }
}

void
Scenario::load()
{
  time_t tmod = fetch_scenario_modification_time();
  if( tmod > scenario_last_modified )
  {
    DLOG_S(INFO) << "Reloading " << main_file_name;    
    
    // do the work of loading the files here 
    valid_scenario = true;
    scenario_has_changed = false;    
    scenario_last_modified = tmod;
  }
  else
  {
    scenario_has_changed = false;
  }
}

bool 
Scenario::load_scenario_file()
{

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

} // namespace sim