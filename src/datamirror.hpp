/*
  Virtual base class for handling of data mirroring from the simulation

  The display sub-system should sub-class this and DataView and implement 
  the apparatus for mirroring data.
*/

namespace sim
{

class DataMirror
// This class is meant to be derived by display devices
{
  virtual DataView& add_orrery_body( std::string name ) = 0;
  virtual DataView& add_space_ship(  std::string name ) = 0;
  // These functions should internally allocate a DataView based class
  // store it in a container and return a reference 
};

} // namespace sim