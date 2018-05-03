#include "spkorrery.hpp"

namespace orrery {

// TODO: Handle files of both endian-ness
bool SpkOrrery::load_orrery_model(
    std::string fname, double begin_jd, double end_jd)
{
    std::ifstream nasa_spk_file(fname, std::ios::binary);

    if (!nasa_spk_file) {
        LOG_S(ERROR) << "Could not open " << fname;
        ok = false;
        return false;
    }

    load_spk(nasa_spk_file, begin_jd, end_jd);

    ok = true;
    return ok;
}

// Fill out the (x, y, z) of each Orrery body and return us an immutable
// vector containing this information.
const OrreryBodyVec& SpkOrrery::get_orrery_at(double jd)
{
    // for (int i = 0; i < bodies.size(); i++) {
    //     segments[i].compute(jd, bodies[i]);
    //     if (segments[i].center != 0) {
    //         bodies[i].pos += bodies[segments[i].center_i].pos;
    //     }
    // }
    return bodies;
}
}