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

  And this stackoverflow answer
  https://space.stackexchange.com/a/20715

  to this question
  https://space.stackexchange.com/questions/12506/what-is-the-exact-format-of-the-jpl-ephemeris-files

*/
#include <fstream>

#include "spk.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace daffile {

/*



*/

const size_t block_size = 1024;

// void SpkSegment::compute(double jd, OrreryBody& body)
// {
// }

const std::string ftp_validation_template
    = "FTPSTR:\r:\n:\r\n:\r\x00:\x81:\x10\xce:ENDFTP";

// There is one file record at the start that has a specific structure reflected
// here
struct FileRecord {
    char      file_architecture[8];   // LOCIDW
    u_int32_t n_double_precision;     // ND
    u_int32_t n_integers;             // NI
    char      internal_name[60];      // LOCIFN
    u_int32_t n_initial_summary;      // FWARD
    u_int32_t n_final_summary;        // BWARD
    u_int32_t first_free_address;     // FREE
    char      numeric_format[8];      // LOCFMT
    char      zeros_1[603];           // PRENUL
    char      ftp_validation_str[28]; // FTPSTR
    char      zeros_2[297];           // PSTNUL
} __attribute__((__packed__));

bool read_file_record(std::ifstream& nasa_spk_file, FileRecord& hdr)
{
    nasa_spk_file.read((char*)(FileRecord*)&hdr, sizeof(hdr));

    LOG_S(INFO) << "File architecture: " << hdr.file_architecture;
    LOG_S(INFO) << "Internal name: " << hdr.internal_name;
    LOG_S(INFO) << "Numeric format: " << hdr.numeric_format;

    if ((hdr.n_double_precision != 2) & (hdr.n_integers != 6)) {
        LOG_S(ERROR) << "Expected ND = 2, NI = 6, but got "
                     << hdr.n_double_precision << ", " << hdr.n_integers;
        return false;
    }

    if (ftp_validation_template.compare(hdr.ftp_validation_str) != 0) {
        LOG_S(ERROR) << "This file is likely corrupted: FTP validation fails";
        return false;
    }

    return true;
}

// 1024 ("block_size") byte blocks are the fundamental unit of a DAF record
// Summary records point to record block numbers ("n_block") that we can use
// to jump to particular records
void read_record_block(
    std::ifstream& nasa_spk_file, size_t n_block, char buf[block_size])
{
    nasa_spk_file.seekg(block_size * (n_block - 1));
    nasa_spk_file.read(buf, block_size);
}

// The DAF comment records are slightly screwy. '\0' is used to indicate a
// new line and '\4' indicates end of comment text. Also, it seems that only
// the first 1000 bytes of the 1024 byte block are actually used.
void parse_daf_comment(char a[block_size])
{
    for (int i = 0; i < 1000; i++) {
        switch (a[i]) {
        case '\0':
            a[i] = '\n';
            break;
        case '\4':
            a[i] = '\0';
            i    = 1000;
            break;
        default:
            break;
        }
    }
}

std::string
read_comment_blocks(std::ifstream& nasa_spk_file, size_t n_initial_summary)
{
    std::string comment;
    for (int i = 2; i < n_initial_summary; i++) {
        char raw_comment[block_size];
        read_record_block(nasa_spk_file, i, raw_comment);
        parse_daf_comment(raw_comment);
        comment += raw_comment;
    }
    return comment;
}

// Summary record header.
// These are integers but they are stored as doubles
struct SummaryRecord {
    double n_next_summary_record;     // pointer to next SR
    double n_previous_summary_record; // pointer to previous SR
    double n_element_records;         // number of element summaries here
} __attribute__((__packed__));

// Summaries
struct ElementSummary {
    double    start_second; // initial epoch, as seconds from J2000
    double    end_second;   // final epoch, as seconds from J2000
    int       target;       // integer target identifier
    int       center;       // integer center identifier
    int       frame;        // integer frame identifier
    int       data_type;    // integer data type identifier
    u_int32_t start_i;      // index where segment starts
    u_int32_t end_i;        // index where segment ends
} __attribute__((__packed__));

std::ostream& operator<<(std::ostream& out, const ElementSummary& es)
{
    out << "(" << es.start_second << " - " << es.end_second
        << ": T:" << es.target << " C:" << es.center << ")";
    return out;
}

typedef std::vector<ElementSummary> ElementSummaryVec;

ElementSummaryVec
read_summaries(std::ifstream& nasa_spk_file, size_t segment_start_block)
{
    SummaryRecord     sr;
    ElementSummaryVec esv;

    size_t summary_records_read = 0;

    nasa_spk_file.seekg(block_size * (segment_start_block - 1));
    nasa_spk_file.read((char*)&sr, sizeof(sr));

    for (;;) {
        LOG_S(INFO) << "Read summary record #" << ++summary_records_read;
        for (size_t n_element = 0; n_element < sr.n_element_records;
             n_element++) {
            ElementSummary es;
            nasa_spk_file.read((char*)&es, sizeof(es));
            esv.push_back(es);
            LOG_S(INFO) << "Read element summary " << n_element + 1 << "/"
                        << sr.n_element_records << " " << es;
        }

        if (sr.n_next_summary_record == 0)
            break;

        nasa_spk_file.seekg(block_size * (sr.n_next_summary_record - 1));
        nasa_spk_file.read((char*)&sr, sizeof(sr));
    }

    return ElementSummaryVec();
}

// size_t read_summary_record(std::ifstream& nasa_spk_file, size_t
// segment_start_block)
// {
//     char buf[block_size];
//     read_record_block(nasa_spk_file, segment_start_block, buf);
//     double next_segment_start_block = *(double*)&buf[0];
//     double n_blocks_in_segment      = *(double*)&buf[16];

//     return (size_t)next_segment_start_block;
// }

struct EphemerisCoeffcients {
};

void load_spk(std::ifstream& nasa_spk_file)
{

    FileRecord hdr;
    if (!read_file_record(nasa_spk_file, hdr)) {
        // ok = false;
        // return false;
    }

    std::string comment
        = read_comment_blocks(nasa_spk_file, hdr.n_initial_summary);
    // std::cout << comment;

    read_summaries(nasa_spk_file, hdr.n_initial_summary);
}
}