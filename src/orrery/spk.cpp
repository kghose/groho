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

  Thanks go to JPL/NASA NAIF docs
  ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/daf.html

  And jplemphem Python code by Brandon Rhodes
  https://github.com/brandon-rhodes/python-jplephem

*/
#include <fstream>

#include "spk.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace orrery {

namespace daffile {

const size_t block_size = 1024;

void SpkSegment::compute(double jd, OrreryBody& body)
{
}

const std::string ftp_validation_template = "FTPSTR:\r:\n:\r\n:\r\x00:\x81:\x10\xce:ENDFTP";

struct FileRecord {
    char      file_architecture[8]; // LOCIDW
    u_int32_t n_double_precision; // ND
    u_int32_t n_integers; // NI
    char      internal_name[60]; // LOCIFN
    u_int32_t n_initial_summary; // FWARD
    u_int32_t n_final_summary; // BWARD
    u_int32_t first_free_address; // FREE
    char      numeric_format[8]; // LOCFMT
    char      zeros_1[603]; //PRENUL
    char      ftp_validation_str[28]; //FTPSTR
    char      zeros_2[297]; //PSTNUL
};

bool read_file_record(std::ifstream& nasa_spk_file, FileRecord& hdr)
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

void parse_daf_comment(char a[block_size])
{
  for(int i = 0; i < 1000; i++) {
    switch (a[i]) {
      case '\0': a[i] = '\n'; break;
      case '\4': a[i] = '\0'; 
      i = 1000;
      break;
      default: break;
    }
  }  
}


std::string read_comment_blocks(std::ifstream& nasa_spk_file, size_t n_initial_summary)
{
  std::string comment;
  nasa_spk_file.seekg( block_size );
  for(int i = 1; i < n_initial_summary - 1; i++) {
    char raw_comment[block_size];
    nasa_spk_file.read(raw_comment, block_size);
    parse_daf_comment(raw_comment);
    comment += raw_comment;
  }
  return comment;
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
    if (!read_file_record(nasa_spk_file, hdr)) {
        ok = false;
        return false;
    }

    std::string comment = read_comment_blocks(nasa_spk_file, hdr.n_initial_summary);
    //std::cout << comment;

    ok = true;
    return ok;
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
