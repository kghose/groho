/*
  This is the interface between the Path object and any system that wants
  to copy over the data for display purposes
*/

struct DataView
// Flattened view (suitable for OpenGL) of data trace in new frame
{
  float*  buf;      // typically will use OpenGL to allocate this pointer
  float*  t_buf;
  size_t  n_points;
  size_t  n_points_allocated;

  std::string          name;
  std::string          reference;

  virtual void allocate( size_t n_pts ) = 0;
};

typedef std::vector<DataView> dv_vec_t;
