/*
  This class sets up a common interface for dealing with Orreries.

  It provides a uniform interface where by
    - We can load Orrery model(s) from disk
    - Obtain a list of GM and (x, y, z) data for all bodies in the Orrery
      at a given time point
*/
#pragma once

#include <string>
#include <vector>

#include "vector.hpp"

namespace orrery {

struct OrreryBody {
    std::string name;
    float       GM;  // GM value for body
    sim::Vector pos; // Position referenced to solar system barycenter
    float       r;   // Radius of body (for collision tests)
};

typedef std::vector<OrreryBody> OrreryBodyVec;

class Orrery {

protected:
    bool          ok;
    OrreryBodyVec bodies;

public:
    Orrery() { ok = false; }
    bool is_ok() { return ok; }

    // Given a model file path, load this orrery file
    // Return false if an error occurs while loading the file
    virtual bool load_orrery_model(std::string fname) = 0;

    // Fill out the (x, y, z) of each Orrery body and return us an immutable
    // vector containing this information.
    virtual const OrreryBodyVec& get_orrery_at(double jd) = 0;
};
}