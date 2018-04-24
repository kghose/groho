/*

  Segments are organized into a list (vector) such that any object that serves
  as a center (reference) for a body appears earlier 
  a 'center' appears earlier in the list than any object tha 
  (such as barycenters) are placed before any bodies that are computed
  with reference to them. 
  
  
  For this reason body 3 (Earth-Moon barycenter) 
  referenced to the solar system barycenter 0, is placed before 399 (Earth)
  or 301 (moon). This sorting is done by placing bodies <= 10 at the begining
  of the list. The order of succeeding bodies is irrelevant 


*/
#include <fstream>

#include "spk.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace orrery {

namespace daffile {

void SpkSegment::compute(double jd, OrreryBody& body)
{
}

//8sII60sIII8s603s28s297s
//            (locidw, self.nd, self.n_integers, self.internal_name, self.initial_record, self.final_record,
//             self.free, numeric_format, self.prenul, self._validation_ftpstr, self.pstnul

const std::string ftp_validation_template = "FTPSTR:\r:\n:\r\n:\r\x00:\x81:\x10\xce:ENDFTP";

struct FileRecord {
    char      file_architecture[8];
    u_int32_t n_double_precision;
    u_int32_t n_integers;
    char      internal_name[60];
    u_int32_t initial_record;
    u_int32_t final_record;
    u_int32_t first_free_address;
    char      numeric_format[8];
    char      zeros_1[603];
    char      ftp_validation_str[28];
    char      zeros_2[297];
};

bool load_file_record(std::ifstream& nasa_spk_file, FileRecord& hdr)
{
    nasa_spk_file.read((char*)(FileRecord*)&hdr, sizeof(hdr));

    LOG_S(INFO) << "File architecture: " << hdr.file_architecture;
    LOG_S(INFO) << "Internal name: " << hdr.internal_name;
    LOG_S(INFO) << "Numeric format: " << hdr.numeric_format;

    if (ftp_validation_template.compare(hdr.ftp_validation_str) != 0) {
        LOG_S(ERROR) << "This file is likely corrupted: FTP validation fails";
        return false;
    }

    return true;
}

//TODO: Handle files of both endian-ness
bool SpkOrrery::load_orrery_model(std::string fname)
{
    std::ifstream nasa_spk_file(fname, std::ios::binary);

    if (!nasa_spk_file) {
        LOG_S(ERROR) << "Could not open " << fname;
        ok = false;
        return false;
    }

    FileRecord hdr;
    if (!load_file_record(nasa_spk_file, hdr)) {
        ok = false;
        return false;
    }

    return true;
}

// Fill out the (x, y, z) of each Orrery body and return us an immutable
// vector containing this information.
const OrreryBodyVec& SpkOrrery::get_orrery_at(double jd)
{
    for (int i = 0; i < bodies.size(); i++) {
        segments[i].compute(jd, bodies[i]);
        if (segments[i].center != 0) {
            bodies[i].pos += bodies[segments[i].center_i].pos;
        }
    }
    return bodies;
}
}
}
