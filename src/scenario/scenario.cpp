#include <fstream>
#include <iostream>
#include <set>

#include "scenario.hpp"
//#include "tokenizedscenario.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace sim {

const std::string wspace = " \t\n\r\f\v";

std::string trim_whitespace(std::string line)
{
    line.erase(0, line.find_first_not_of(wspace));
    line.erase(line.find_last_not_of(wspace) + 1);
    return line;
}

std::string trim_comments(std::string line)
{
    return line.erase(std::min(line.find_first_of(';'), line.size()));
}

std::optional<KeyValue> get_key_value(std::string line)
{
    KeyValue kv;
    size_t   n0 = line.find_first_of('=');

    if (n0 > line.size()) {
        return {};
    }

    kv.key   = trim_whitespace(line.substr(0, n0 - 1));
    kv.value = trim_whitespace(line.substr(n0 + 1));
    return kv;
}

std::optional<Configuration> parse_configuration(std::string fname)
{

    std::ifstream cfile(fname);
    int           line_no = 0;
    std::string   line;
    Configuration cfg;

    // LOG_S(INFO) << "Parsing scenario file " << fname;

    while (std::getline(cfile, line)) {
        line_no++;

        line = trim_whitespace(trim_comments(line));

        if (line.length() == 0) // Ignore empty lines
            continue;

        std::optional<KeyValue> kv = get_key_value(line);
        if (kv) {
            if (!cfg.set_key_value(kv)) {
                LOG_S(WARNING) << "Line " << line_no << ": Unknown key "
                               << kv->key << ": ignoring";
            }
        } else {
            LOG_S(ERROR) << "Error parsing line: " << line_no << " (" << line
                         << ")";
        }
    }
    return cfg;
}

bool operator==(const Configuration& a, const Configuration& b)
{
    if (a.begin_jd != b.begin_jd)
        return false;
    if (a.end_jd != b.end_jd)
        return false;
    if (a.step != b.step)
        return false;
    if (a.orrery != b.orrery)
        return false;
    if (a.flightplan != b.flightplan)
        return false;
    return true;
}

bool operator==(const Scenario& a, const Scenario& b)
{
    if (*a.configuration != *b.configuration)
        return false;
    return true;
}

} // namespace sim