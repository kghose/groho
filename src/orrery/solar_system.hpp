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

/*
Data is arranged to match OrreryBody

struct Body {
    BodyType    body_type;
    int         code; // SPK code for body (made up ones for spaceships)
    std::string name; // Human readable name for body

    float GM; // GM value for body
    float r;  // Radius of body (for collision tests)

    uint32_t color; // For display purposes

    float max_acc;   // Maximum acceleration possible for ship m/s^2
    float acc;       // Current acceleration m/s^2
    float max_fuel;  // Maximum fuel reserve (U)
    float fuel;      // Current fuel reserve (U)
    float fuel_cons; // Fuel consumption rate (U/ (m/s^2))

    FlightState flight_state;

    sim::Vector pos; // Position referenced to solar system barycenter
    sim::Vector vel; // Velocity
    sim::Vector att; // Attitude
};

*/

using namespace sim;

// clang-format off
std::unordered_map<int, OrreryBody> body_library = {

    // These are not present, physcially, in the simulation, being barycenters
    {       1, { BARYCENTER,       1, "Mercury BC",  2.2031780000000021E+04,            0, 0x000000 } },
    {       2, { BARYCENTER,       2, "Venus BC",    3.2485859200000006E+05,            0, 0x000000 } },
    {       3, { BARYCENTER,       3, "Earth BC",    4.0350323550225981E+05,            0, 0x000000 } },
    {       4, { BARYCENTER,       4, "Mars BC",     4.2828375214000022E+04,            0, 0x000000 } },
    {       5, { BARYCENTER,       5, "Jupiter BC",  1.2671276480000021E+08,            0, 0x000000 } },
    {       6, { BARYCENTER,       6, "Saturn BC",   3.7940585200000003E+07,            0, 0x000000 } },
    {       7, { BARYCENTER,       7, "Uranus BC",   5.7945486000000080E+06,            0, 0x000000 } },
    {       8, { BARYCENTER,       8, "Neptune BC",  6.8365271005800236E+06,            0, 0x000000 } },
    {       9, { BARYCENTER,       9, "Pluto BC",    9.7700000000000068E+02,            0, 0x000000 } },
    {      10, {       ROCK,      10, "Sun",         1.3271244004193938E+11,  696342000.0, 0xFFFF00 } },
    {     199, {       ROCK,     199, "Mercury",     2.2031780000000021E+04,    2439700.0, 0x808080 } },
    {     299, {       ROCK,     299, "Venus",       3.2485859200000006E+05,    6051800.0, 0xFFFFE0 } },
    {     399, {       ROCK,     399, "Earth",       3.9860043543609598E+05,    6378140.0, 0x0000FF } },
    {     301, {       ROCK,     301, "Moon",        4.9028000661637961E+03,    1737400.0, 0xffffff } },
    {     499, {       ROCK,     499, "Mars",        4.2828373620699090E+04,    3396190.0, 0xB22222 } },
    {     401, {       ROCK,     401, "Phobos",      7.0875460668944520E-04,      13000.0, 0x808080 } },
    {     402, {       ROCK,     402, "Deimos",      9.6155696481203130E-05,       7800.0, 0x808080 } },
    {     599, {       ROCK,     599, "Jupiter",     1.2668653492180080E+08,   71492000.0, 0xFFA500 } },
    {     501, {       ROCK,     501, "Io",          5.9599160334104040E+03,    1829400.0, 0x808080 } },
    {     502, {       ROCK,     502, "Europa",      3.2027387749228920E+03,    1562600.0, 0x808080 } },
    {     503, {       ROCK,     503, "Ganymede",    9.8878344533341440E+03,    2631200.0, 0x808080 } },
    {     504, {       ROCK,     504, "Callisto",    7.1792893613972700E+03,    2410300.0, 0x808080 } },
    {     505, {       ROCK,     505, "Amalthea",    1.3784805712026150E-01,     125000.0, 0x808080 } },
    {     506, {       ROCK,     506, "Himalia",     1.3800806960789660E-01,      85000.0, 0x808080 } },
    {     699, {       ROCK,     699, "Saturn",      3.7931207498652240E+07,   60268000.0, 0xFFD700 } },
    {     601, {       ROCK,     601, "Mimas",       2.5035228846617950E+00,     207800.0, 0x808080 } },
    {     602, {       ROCK,     602, "Enceladus",   7.2112920854799890E+00,     256600.0, 0x808080 } },
    {     603, {       ROCK,     603, "Tethys",      4.1211172077013020E+01,     538400.0, 0x808080 } },
    {     604, {       ROCK,     604, "Dione",       7.3116353229231930E+01,     563400.0, 0x808080 } },
    {     605, {       ROCK,     605, "Rhea",        1.5394220455453420E+02,     765000.0, 0x808080 } },
    {     606, {       ROCK,     606, "Titan",       8.9781388453073760E+03,    2575150.0, 0x808080 } },
    {     607, {       ROCK,     607, "Hyperion",    3.7187917141916680E-01,     180100.0, 0x808080 } },
    {     608, {       ROCK,     608, "Iapetus",     1.2051347817240410E+02,     745700.0, 0x808080 } },
    {     609, {       ROCK,     609, "Phoebe",      5.5311104146333740E-01,     109400.0, 0x808080 } },
    {     610, {       ROCK,     610, "Janus",       1.2662312969456360E-01,     101500.0, 0x808080 } },
    {     611, {       ROCK,     611, "Epimetheus",  3.5139774905684570E-02,      64900.0, 0x808080 } },
    {     615, {       ROCK,     615, "Atlas",       3.7597188869653530E-04,      20400.0, 0x808080 } },
    {     616, {       ROCK,     616, "Prometheus",  1.0663684266661340E-02,      67800.0, 0x808080 } },
    {     617, {       ROCK,     617, "Pandora",     9.1037683110543000E-03,      52000.0, 0x808080 } },
    {     799, {       ROCK,     799, "Uranus",      5.7939513222790090E+06,   25559000.0, 0xADD8E6 } },
    {     701, {       ROCK,     701, "Ariel",       8.3463444317704770E+01,     581100.0, 0x808080 } },
    {     702, {       ROCK,     702, "Umbriel",     8.5093380944893880E+01,     584700.0, 0x808080 } },
    {     703, {       ROCK,     703, "Titania",     2.2694370037412480E+02,     788900.0, 0x808080 } },
    {     704, {       ROCK,     704, "Oberon",      2.0532343025356230E+02,     761400.0, 0x808080 } },
    {     705, {       ROCK,     705, "Miranda",     4.3195168992321000E+00,     240400.0, 0x808080 } },
    {     899, {       ROCK,     899, "Neptune",     6.8350995024396720E+06,   24764000.0, 0xADD8E6 } },
    {     801, {       ROCK,     801, "Triton",      1.4275981407250340E+03,    1352600.0, 0x808080 } },
    {     999, {       ROCK,     999, "Pluto",       8.6961381776087480E+02,    1195000.0, 0xF4A460 } },
    {     901, {       ROCK,     901, "Charon",      1.0587998886018810E+02,     605000.0, 0x808080 } },
    {     902, {       ROCK,     902, "Nix",         3.0481756481697600E-03,      17500.0, 0x808080 } },
    {     903, {       ROCK,     903, "Hydra",       3.2110392061552550E-03,      19000.0, 0x808080 } },
    {     904, {       ROCK,     904, "Kerberos",    1.1100408505366760E-03,      10000.0, 0x808080 } },
    { 2000001, {       ROCK, 2000001, "Ceres",       6.3130000000000003E+01,     469700.0, 0x808080 } },
    { 2000002, {       ROCK, 2000002, "Pallas",      1.3730000000000000E+01,     272500.0, 0x808080 } },
    { 2000003, {       ROCK, 2000003, "Juno",        1.8200000000000001E+00,     123298.0, 0x808080 } },
    { 2000004, {       ROCK, 2000004, "Vesta",       1.7289999999999999E+01,     262700.0, 0x808080 } },
    { 2000006, {       ROCK, 2000006, "Hebe",        9.3000000000000005E-01,      92590.0, 0x808080 } },
    { 2000007, {       ROCK, 2000007, "Iris",        8.5999999999999999E-01,      99906.0, 0x808080 } },
    { 2000010, {       ROCK, 2000010, "Hygiea",      5.7800000000000002E+00,     203560.0, 0x808080 } },
    { 2000015, {       ROCK, 2000015, "Eunomia",     2.1000000000000001E+00,     115845.0, 0x808080 } },
    { 2000016, {       ROCK, 2000016, "Psyche",      1.8100000000000001E+00,     113000.0, 0x808080 } },
    { 2000029, {       ROCK, 2000029, "Amphitrite",  8.5999999999999999E-01,      94779.0, 0x808080 } },
    { 2000052, {       ROCK, 2000052, "Europa",      1.5900000000000001E+00,     151959.0, 0x808080 } },
    { 2000065, {       ROCK, 2000065, "Cybele",      9.1000000000000003E-01,     118630.0, 0x808080 } },
    { 2000087, {       ROCK, 2000087, "Sylvia",      9.8999999999999999E-01,     126526.0, 0x808080 } }, // discovered at Madras
    { 2000088, {       ROCK, 2000088, "Thisbe",      1.0200000000000000E+00,     116000.0, 0x808080 } },
    { 2000433, {       ROCK, 2000433, "Eros",        4.463E-4              ,       8420.0, 0x808080 } },
    { 2000511, {       ROCK, 2000511, "Davida",      2.2599999999999998E+00,     135164.0, 0x808080 } },
    { 2000704, {       ROCK, 2000704, "Interamnia",  2.1899999999999999E+00,     153156.0, 0x808080 } }
};

}