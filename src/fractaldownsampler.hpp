namespace sim
{

class FractalDownsampler
{
public:
  FractalDownsampler( double rt=1.001, double lt=1e6 ) : 
      ratio_threshold( rt ), linear_threshold( lt )
  {}

  bool operator()( Vector& v )
  {
    cumulative_curve_dist += (v - last_v).norm();
    float linear_dist = (v - last_sample_v).norm();
     
    if(  ( ( cumulative_curve_dist / linear_dist )    > ratio_threshold  )   
       | ( abs( cumulative_curve_dist - linear_dist ) > linear_threshold ) ) 
    {
      cumulative_curve_dist = 0;
      last_sample_v = v;
      last_v = v;
      return true;
    }
    last_v = v;
    return false;
  }

private:
  double     cumulative_curve_dist = 0;
  Vector     last_sample_v = { 0, 0, 0 };
  Vector     last_v = { 0, 0, 0 };
  double     ratio_threshold = 1.001;
  double     linear_threshold = 1e6;
};

} // namespace sim