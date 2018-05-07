/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

The following code stores a mapping from a SPK ID of a solar system bod to a
structure carrying information about its name, GM and radius

This table has been compiled from:

Names correspondng to SPK IDs: [NAIF/JPL/NASA][naif] and, for the smaller
bodies, [SSD/JPL/NASA][ssd]

GM: [NAIF/JPL/NASA][naif2]

Radii from [NAIF/JPL/NASA][naif3] and for the three moons of Pluto, from
[wikipedia][wp]


[naif]:
https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/naif_ids.html#NAIF%20Object%20ID%20numbers
[ssd]: https://ssd.jpl.nasa.gov/sbdb.cgi

[naif2]: https://naif.jpl.nasa.gov/pub/naif/generic_kernels/pck/gm_de431.tpc

[naif3]: https://naif.jpl.nasa.gov/pub/naif/generic_kernels/pck/pck00010.tpc

[wp]: https://en.wikipedia.org/wiki/List_of_Solar_System_objects_by_size
*/

#pragma once

#include <unordered_map>

#include "orrery.hpp"

namespace orrery {

// Data is arranged to match OrreryBody
//
// struct OrreryBody {
//     std::string name; // Human readable name for body
//     int         code; // SPK code for body (optional)
//     float       GM;   // GM value for body
//     float       r;    // Radius of body (for collision tests)
//     bool real_body; // Is this a real solar system body for display purposes
//                        and gravitational pull computations ?
//     sim::Vector pos; // Position referenced to solar system barycenter
// };
//

// clang-format off
std::unordered_map<int, OrreryBody> body_library = {

    // These are not present, physcially, in the simulation, being barycenters
    { 1  , { "Mercury BC", 1  , 2.2031780000000021E+04,  0, false, { 0, 0, 0} } },
    { 2  , { "Venus BC",   2  , 3.2485859200000006E+05,  0, false, { 0, 0, 0} } },
    { 3  , { "Earth BC",   3  , 4.0350323550225981E+05,  0, false, { 0, 0, 0} } },
    { 4  , { "Mars BC",    4  , 4.2828375214000022E+04,  0, false, { 0, 0, 0} } },
    { 5  , { "Jupiter BC", 5  , 1.2671276480000021E+08,  0, false, { 0, 0, 0} } },
    { 6  , { "Saturn BC",  6  , 3.7940585200000003E+07,  0, false, { 0, 0, 0} } },
    { 7  , { "Uranus BC",  7  , 5.7945486000000080E+06,  0, false, { 0, 0, 0} } },
    { 8  , { "Neptune BC", 8  , 6.8365271005800236E+06,  0, false, { 0, 0, 0} } },
    { 9  , { "Pluto BC",   9  , 9.7700000000000068E+02,  0, false, { 0, 0, 0} } },

    { 10 , { "Sun",        10 , 1.3271244004193938E+11,  696342000.0, true, { 0, 0, 0} } },

    { 199 , { "Mercury",   199, 2.2031780000000021E+04,    2439700.0, true, { 0.0, 0.0, 0.0 } } },

    { 299 , { "Venus",     299, 3.2485859200000006E+05,    6051800.0, true, { 0.0, 0.0, 0.0 } } },

    { 399 , { "Earth",     399, 3.9860043543609598E+05,    6378140.0, true, { 0.0, 0.0, 0.0 } } },
    { 301 , { "Moon",      301, 4.9028000661637961E+03,    1737400.0, true, { 0.0, 0.0, 0.0 } } },

    { 499 , { "Mars",      499, 4.282837362069909E+04,     3396190.0, true, { 0.0, 0.0, 0.0 } } },
    { 401 , { "Phobos",    401, 7.087546066894452E-04,       13000.0, true, { 0.0, 0.0, 0.0 } } },
    { 402 , { "Deimos",    402, 9.615569648120313E-05,        7800.0, true, { 0.0, 0.0, 0.0 } } },

    { 599 , { "Jupiter",   599, 1.266865349218008E+08,    71492000.0, true, { 0.0, 0.0, 0.0 } } },
    { 501 , { "Io",        501, 5.959916033410404E+03,     1829400.0, true, { 0.0, 0.0, 0.0 } } },
    { 502 , { "Europa",    502, 3.202738774922892E+03,     1562600.0, true, { 0.0, 0.0, 0.0 } } },
    { 503 , { "Ganymede",  503, 9.887834453334144E+03,     2631200.0, true, { 0.0, 0.0, 0.0 } } },
    { 504 , { "Callisto",  504, 7.179289361397270E+03,     2410300.0, true, { 0.0, 0.0, 0.0 } } },
    { 505 , { "Amalthea",  505, 1.378480571202615E-01,      125000.0, true, { 0.0, 0.0, 0.0 } } },
    { 506,  { "Himalia",   506, 1.380080696078966E-01,       85000.0, true, { 0.0, 0.0, 0.0 } } },

    { 699 , { "Saturn",    699, 3.793120749865224E+07 ,   60268000.0, true, { 0.0, 0.0, 0.0 } } },
    { 601 , { "Mimas",     601, 2.503522884661795E+00 ,     207800.0, true, { 0.0, 0.0, 0.0 } } },
    { 602 , { "Enceladus", 602, 7.211292085479989E+00 ,     256600.0, true, { 0.0, 0.0, 0.0 } } },
    { 603 , { "Tethys",    603, 4.121117207701302E+01 ,     538400.0, true, { 0.0, 0.0, 0.0 } } },
    { 604 , { "Dione",     604, 7.311635322923193E+01 ,     563400.0, true, { 0.0, 0.0, 0.0 } } },
    { 605 , { "Rhea",      605, 1.539422045545342E+02 ,     765000.0, true, { 0.0, 0.0, 0.0 } } },
    { 606 , { "Titan",     606, 8.978138845307376E+03 ,    2575150.0, true, { 0.0, 0.0, 0.0 } } },
    { 607 , { "Hyperion",  607, 3.718791714191668E-01 ,     180100.0, true, { 0.0, 0.0, 0.0 } } },
    { 608 , { "Iapetus",   608, 1.205134781724041E+02 ,     745700.0, true, { 0.0, 0.0, 0.0 } } },
    { 609 , { "Phoebe",    609, 5.531110414633374E-01 ,     109400.0, true, { 0.0, 0.0, 0.0 } } },
    { 610 , { "Janus",     610, 1.266231296945636E-01 ,     101500.0, true, { 0.0, 0.0, 0.0 } } },
    { 611 , { "Epimetheus",611, 3.513977490568457E-02 ,      64900.0, true, { 0.0, 0.0, 0.0 } } },
    { 615 , { "Atlas",     615, 3.759718886965353E-04 ,      20400.0, true, { 0.0, 0.0, 0.0 } } },
    { 616 , { "Prometheus",616, 1.066368426666134E-02 ,      67800.0, true, { 0.0, 0.0, 0.0 } } },
    { 617 , { "Pandora",   617, 9.103768311054300E-03 ,      52000.0, true, { 0.0, 0.0, 0.0 } } },

    { 799 , { "Uranus",    799, 5.793951322279009E+06 ,   25559000.0, true, { 0.0, 0.0, 0.0 } } },
    { 701 , { "Ariel",     701, 8.346344431770477E+01 ,     581100.0, true, { 0.0, 0.0, 0.0 } } },
    { 702 , { "Umbriel",   702, 8.509338094489388E+01 ,     584700.0, true, { 0.0, 0.0, 0.0 } } },
    { 703 , { "Titania",   703, 2.269437003741248E+02 ,     788900.0, true, { 0.0, 0.0, 0.0 } } },
    { 704 , { "Oberon",    704, 2.053234302535623E+02 ,     761400.0, true, { 0.0, 0.0, 0.0 } } },
    { 705 , { "Miranda",   705, 4.319516899232100E+00 ,     240400.0, true, { 0.0, 0.0, 0.0 } } },

    { 899 , { "Neptune",   899, 6.835099502439672E+06 ,   24764000.0, true, { 0.0, 0.0, 0.0 } } },
    { 801 , { "Triton",    801, 1.427598140725034E+03 ,    1352600.0, true, { 0.0, 0.0, 0.0 } } },

    { 999 , { "Pluto",     999, 8.696138177608748E+02 ,    1195000.0, true, { 0.0, 0.0, 0.0 } } },
    { 901 , { "Charon",    901, 1.058799888601881E+02 ,     605000.0, true, { 0.0, 0.0, 0.0 } } },
    { 902 , { "Nix",       902, 3.048175648169760E-03 ,      17500.0, true, { 0.0, 0.0, 0.0 } } },
    { 903 , { "Hydra",     903, 3.211039206155255E-03 ,      19000.0, true, { 0.0, 0.0, 0.0 } } },
    { 904 , { "Kerberos",  904, 1.110040850536676E-03 ,      10000.0, true, { 0.0, 0.0, 0.0 } } },

    { 2000001, { "Ceres",   2000001, 6.3130000000000003E+01, 469700, true, { 0.0, 0.0, 0.0} } },
    { 2000002, { "Pallas",  2000002, 1.3730000000000000E+01, 272500, true, { 0.0, 0.0, 0.0} } },
    { 2000003, { "Juno",    2000003, 1.8200000000000001E+00, 123298, true, { 0.0, 0.0, 0.0} } },
    { 2000004, { "Vesta",   2000004, 1.7289999999999999E+01, 262700, true, { 0.0, 0.0, 0.0} } },
    { 2000006, { "Hebe",    2000006, 9.3000000000000005E-01,  92590, true, { 0.0, 0.0, 0.0} } },
    { 2000007, { "Iris",    2000007, 8.5999999999999999E-01,  99906, true, { 0.0, 0.0, 0.0} } },
    { 2000010, { "Hygiea",  2000010, 5.7800000000000002E+00, 203560, true, { 0.0, 0.0, 0.0} } },
    { 2000015, { "Eunomia", 2000015, 2.1000000000000001E+00, 115845, true, { 0.0, 0.0, 0.0} } },
    { 2000016, { "Psyche",  2000016, 1.8100000000000001E+00, 113000, true, { 0.0, 0.0, 0.0} } },
    { 2000029, { "Amphitrite",2000029, 8.5999999999999999E-01, 94779, true, { 0.0, 0.0, 0.0} } },
    { 2000052, { "Europa",  2000052, 1.5900000000000001E+00, 151959, true, { 0.0, 0.0, 0.0} } },
    { 2000065, { "Cybele",  2000065, 9.1000000000000003E-01, 118630, true, { 0.0, 0.0, 0.0} } },
    { 2000087, { "Sylvia",  2000087, 9.8999999999999999E-01, 126526, true, { 0.0, 0.0, 0.0} } }, // discovered at Madras
    { 2000088, { "Thisbe",  2000088, 1.0200000000000000E+00, 116000, true, { 0.0, 0.0, 0.0} } },
    { 2000433, { "Eros",    2000433, 4.463E-4              ,   8420, true, { 0.0, 0.0, 0.0} } },
    { 2000511, { "Davida",  2000511, 2.2599999999999998E+00, 135164, true, { 0.0, 0.0, 0.0} } },
    { 2000704, { "Interamnia", 2000704, 2.1899999999999999E+00, 153156, true, { 0.0, 0.0, 0.0} } }
};

}