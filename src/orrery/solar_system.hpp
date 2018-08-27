/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2018 by Kaushik Ghose. Some rights reserved, see LICENSE

The following code stores a mapping from a SPK ID of a solar system bod to a
structure carrying information about its name, GM and radius

A very comprehensive table is found at https://ssd.jpl.nasa.gov/?sat_phys_par

However, before I found this table, I scrabbled through the following sources:

Names correspondng to SPK IDs: [NAIF/JPL/NASA][naif] and, for the smaller
bodies, [SSD/JPL/NASA][ssd]

GM: [NAIF/JPL/NASA][naif2]

Radii from [NAIF/JPL/NASA][naif3] and for the three moons of Pluto, from
[wikipedia][wp]

For some of Jupiter's [moons][jupiter]

[naif]:
https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/naif_ids.html#NAIF%20Object%20ID%20numbers
[ssd]: https://ssd.jpl.nasa.gov/sbdb.cgi

[naif2]: https://naif.jpl.nasa.gov/pub/naif/generic_kernels/pck/gm_de431.tpc

[naif3]: https://naif.jpl.nasa.gov/pub/naif/generic_kernels/pck/pck00010.tpc

[wp]: https://en.wikipedia.org/wiki/List_of_Solar_System_objects_by_size

[jupiter]:
https://naif.jpl.nasa.gov/pub/naif/pds/data/co-s_j_e_v-spice-6-v1.0/cosp_1000/data/pck/cpck30Sep2004_jupiter.tpc
*/

#pragma once

#include <unordered_map>
#include <vector>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

#include "body.hpp"

namespace sim {

struct OrreryBodyData {
    int         naif_code;
    std::string body_name;

    float GM; // GM value for body
    float r;  // Radius of body (for collision tests)

    uint32_t color; // For display purposes
};

// clang-format off
std::vector<OrreryBodyData> body_library = {
    //                                 GM                   R
    //                               km^3 s^−2             km 
    {       1, "Mercury BC",  2.2031780000000021E+04,            0, 0x000000 },
    {       2, "Venus BC",    3.2485859200000006E+05,            0, 0x000000 },
    {       3, "Earth BC",    4.0350323550225981E+05,            0, 0x000000 },
    {       4, "Mars BC",     4.2828375214000022E+04,            0, 0x000000 },
    {       5, "Jupiter BC",  1.2671276480000021E+08,            0, 0x000000 },
    {       6, "Saturn BC",   3.7940585200000003E+07,            0, 0x000000 },
    {       7, "Uranus BC",   5.7945486000000080E+06,            0, 0x000000 },
    {       8, "Neptune BC",  6.8365271005800236E+06,            0, 0x000000 },
    {       9, "Pluto BC",    9.7700000000000068E+02,            0, 0x000000 },
    {      10, "Sun",         1.3271244004193938E+11,   696342.000, 0xFFFF00 },
    {     199, "Mercury",     2.2031780000000021E+04,     2439.700, 0x808080 },
    {     299, "Venus",       3.2485859200000006E+05,     6051.800, 0xFFFFE0 },
    {     399, "Earth",       3.9860043543609598E+05,     6378.140, 0x0000FF },
    {     301, "Moon",        4.9028000661637961E+03,     1737.400, 0xffffff },
    {     499, "Mars",        4.2828373620699090E+04,     3396.190, 0xB22222 },
    {     401, "Phobos",      7.0875460668944520E-04,       13.000, 0x808080 },
    {     402, "Deimos",      9.6155696481203130E-05,        7.800, 0x808080 },
    {     599, "Jupiter",     1.2668653492180080E+08,    71492.000, 0xFFA500 },
    {     501, "Io",          5.9599160334104040E+03,     1829.400, 0x808080 },
    {     502, "Europa",      3.2027387749228920E+03,     1562.600, 0x808080 },
    {     503, "Ganymede",    9.8878344533341440E+03,     2631.200, 0x808080 },
    {     504, "Callisto",    7.1792893613972700E+03,     2410.300, 0x808080 },
    {     505, "Amalthea",    1.3784805712026150E-01,      125.000, 0x808080 },
    {     506, "Himalia",     1.3800806960789660E-01,       85.000, 0x808080 },
    {     514, "Thebe",       1.0000000000000000E-01,       49.000, 0x808080 },
    {     515, "Adrastea",    5.0000000000000000E-04,        8.000, 0x808080 },
    {     516, "Metis",       8.0000000000000000E-03,       21.500, 0x808080 },
    {     699, "Saturn",      3.7931207498652240E+07,    60268.000, 0xFFD700 },
    {     601, "Mimas",       2.5035228846617950E+00,      207.800, 0x808080 },
    {     602, "Enceladus",   7.2112920854799890E+00,      256.600, 0x808080 },
    {     603, "Tethys",      4.1211172077013020E+01,      538.400, 0x808080 },
    {     604, "Dione",       7.3116353229231930E+01,      563.400, 0x808080 },
    {     605, "Rhea",        1.5394220455453420E+02,      765.000, 0x808080 },
    {     606, "Titan",       8.9781388453073760E+03,     2575.150, 0x808080 },
    {     607, "Hyperion",    3.7187917141916680E-01,      180.100, 0x808080 },
    {     608, "Iapetus",     1.2051347817240410E+02,      745.700, 0x808080 },
    {     609, "Phoebe",      5.5311104146333740E-01,      109.400, 0x808080 },
    {     610, "Janus",       1.2662312969456360E-01,      101.500, 0x808080 },
    {     611, "Epimetheus",  3.5139774905684570E-02,       64.900, 0x808080 },
    {     615, "Atlas",       3.7597188869653530E-04,       20.400, 0x808080 },
    {     616, "Prometheus",  1.0663684266661340E-02,       67.800, 0x808080 },
    {     617, "Pandora",     9.1037683110543000E-03,       52.000, 0x808080 },
    {     799, "Uranus",      5.7939513222790090E+06,    25559.000, 0xADD8E6 },
    {     701, "Ariel",       8.3463444317704770E+01,      581.100, 0x808080 },
    {     702, "Umbriel",     8.5093380944893880E+01,      584.700, 0x808080 },
    {     703, "Titania",     2.2694370037412480E+02,      788.900, 0x808080 },
    {     704, "Oberon",      2.0532343025356230E+02,      761.400, 0x808080 },
    {     705, "Miranda",     4.3195168992321000E+00,      240.400, 0x808080 },
    {     899, "Neptune",     6.8350995024396720E+06,    24764.000, 0xADD8E6 },
    {     801, "Triton",      1.4275981407250340E+03,     1352.600, 0x808080 },
    {     802, "Nereid",      2.0600000000000000E+00,      170.000, 0x808080 },
    {     808, "Proteus",     3.3600000000000000E+00,      210.000, 0x808080 },
    {     999, "Pluto",       8.6961381776087480E+02,     1195.000, 0xF4A460 },
    {     901, "Charon",      1.0587998886018810E+02,      605.000, 0x808080 },
    {     902, "Nix",         3.0481756481697600E-03,       17.500, 0x808080 },
    {     903, "Hydra",       3.2110392061552550E-03,       19.000, 0x808080 },
    {     904, "Kerberos",    1.1100408505366760E-03,       14.000, 0x808080 },
    {     905, "Styx",        8.0000000000000000E-04,       10.000, 0x808080 }, // Given as zero
    { 2000001, "Ceres",       6.3130000000000003E+01,      469.700, 0x808080 },
    { 2000002, "Pallas",      1.3730000000000000E+01,      272.500, 0x808080 },
    { 2000003, "Juno",        1.8200000000000001E+00,      123.298, 0x808080 },
    { 2000004, "Vesta",       1.7289999999999999E+01,      262.700, 0x808080 },
    { 2000006, "Hebe",        9.3000000000000005E-01,       92.590, 0x808080 },
    { 2000007, "Iris",        8.5999999999999999E-01,       99.906, 0x808080 },
    { 2000010, "Hygiea",      5.7800000000000002E+00,      203.560, 0x808080 },
    { 2000015, "Eunomia",     2.1000000000000001E+00,      115.845, 0x808080 },
    { 2000016, "Psyche",      1.8100000000000001E+00,      113.000, 0x808080 },
    { 2000029, "Amphitrite",  8.5999999999999999E-01,       94.779, 0x808080 },
    { 2000052, "Europa",      1.5900000000000001E+00,      151.959, 0x808080 },
    { 2000065, "Cybele",      9.1000000000000003E-01,      118.630, 0x808080 },
    { 2000087, "Sylvia",      9.8999999999999999E-01,      126.526, 0x808080 }, // discovered at Madras
    { 2000088, "Thisbe",      1.0200000000000000E+00,      116.000, 0x808080 },
    { 2000433, "Eros",        4.463E-4              ,        8.420, 0x808080 },
    { 2000511, "Davida",      2.2599999999999998E+00,      135.164, 0x808080 },
    { 2000704, "Interamnia",  2.1899999999999999E+00,      153.156, 0x808080 }
};

// clang-format on

inline std::unordered_map<int, OrreryBodyData> create_library_map()
{
    std::unordered_map<int, OrreryBodyData> bl;
    for (auto& b : body_library) {
        bl[b.naif_code] = b;
    }
    return bl;
}

inline RockLike::Property construct_orrery_body(int naif_id)
{
    static std::unordered_map<int, OrreryBodyData> lib = create_library_map();
    if (lib.count(naif_id) > 0) {
        auto& bd = lib[naif_id];
        return { NAIFbody(bd.naif_code, bd.body_name), bd.GM, bd.r, bd.color };
    } else {
        LOG_S(WARNING) << naif_id << " not found in solar system library";
        return { NAIFbody(naif_id, std::to_string(naif_id)), 0, 1.0, 0xFFFF00 };
    }
}
}