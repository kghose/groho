/*


*/

namespace sim {

class Planetary_orbit_engine {
    public:
    virtual bool initialize(const std::string data_file_name);        
    virtual bool body_position(const uint64_t teph, const std::string body_name); 
};

// From http://ssd.jpl.nasa.gov/txt/aprx_pos_planets.pdf
class NASA_keplerian : public Planetary_orbit_engine {
    public:
    bool initialize(const std::string data_file_name);        
    bool body_position(const uint64_t teph, const std::string body_name);
}

}
