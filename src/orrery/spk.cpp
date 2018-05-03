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
#include <cmath>
#include <fstream>
#include <optional>

#include "spk.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace daffile {

const double T0        = 2451545.0; // Julian date for the year 2000
const double S_PER_DAY = 86400.0;

double jd_to_j2000(double jd) { return (jd - T0) * S_PER_DAY; }

double j2000_to_jd(double seconds) { return T0 + seconds / S_PER_DAY; }

// https://en.wikipedia.org/wiki/Earth-centered_inertial#J2000
// One commonly used ECI frame is defined with the Earth's Mean Equator and
// Equinox at 12:00 Terrestrial Time on 1 January 2000. It can be referred to as
// J2000 or EME2000. The x-axis is aligned with the mean equinox. The z-axis is
// aligned with the Earth's spin axis or celestial North Pole. The y-axis is
// rotated by 90° East about the celestial equator.[5]

// In the following section all structures that closely reflect some part of the
// SPK/DAF file structure are named adhering to the nomenclature JPL/NASA
// adopted. The final structure (Emphemeris) which collates and organizes the
// data in a manner suitable for this program is named to match it's logical
// function.

//   Thanks go to JPL/NASA NAIF docs
//   ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/daf.html

//   the jplemphem Python code by Brandon Rhodes
//   https://github.com/brandon-rhodes/python-jplephem

//   And this stackoverflow answer
//   https://space.stackexchange.com/a/20715
//   to this question
//   https://space.stackexchange.com/questions/12506/what-is-the-exact-format-of-the-jpl-ephemeris-files

// The file is organized in 1024 byte blocks
const size_t block_size = 1024;

// 8 byte doubles are the standard data for most of the file
const size_t size_of_double = 8;

const std::string integrity_check_template
    = "FTPSTR:\r:\n:\r\n:\r\x00:\x81:\x10\xce:ENDFTP";

// There is one file record at the start that has a specific structure reflected
// here spanning, 1024 bytes
struct FileRecord {
    char      file_architecture[8]; // LOCIDW
    u_int32_t n_double_precision;   // ND
    u_int32_t n_integers;           // NI
    char      internal_name[60];    // LOCIFN
    u_int32_t first_summary_block;  // FWARD
    u_int32_t last_summary_block;   // BWARD
    u_int32_t first_free_address;   // FREE
    char      numeric_format[8];    // LOCFMT
    char      zeros_1[603];         // PRENUL
    char      integrity_string[28]; // FTPSTR
    char      zeros_2[297];         // PSTNUL
} __attribute__((__packed__));

std::optional<FileRecord> read_file_record(std::ifstream& nasa_spk_file);

// There is usually a comments section after this which is free form, taking
// one or more 1024 byte blocks.

std::string read_comment_blocks(
    std::ifstream& nasa_spk_file, std::optional<FileRecord> hdr);

// The comment records are followed by summary records.
// Blocks of summary records are chained like a linked list with each block
// having a header section that carries the chain information. These values are
// integers but they are stored as doubles
struct SummaryRecordBlockHeader {
    double next_summary_record_blk; // pointer to next SR 1Kb block (1 indexed)
    double prev_summary_record_blk; // pointer to previous SR block (1 indexed)
    double n_summaries;             // number of element summaries here
} __attribute__((__packed__));

// Each summary has a fixed form, reflected here
struct Summary {
    double    begin_second; // initial epoch, as seconds from J2000
    double    end_second;   // final epoch, as seconds from J2000
    u_int32_t target_id;    // target identifier
    u_int32_t center_id;    // center identifier
    u_int32_t frame_id;     // frame identifier (we handle 1 - J2000 - only)
    u_int32_t data_type;    // data type identifier (we handle II or III)
    u_int32_t start_i;      // index (8 byte blocks) where segment data starts
    u_int32_t end_i;        // index (8 byte blocks) where segment data ends
} __attribute__((__packed__));

typedef std::vector<Summary> SummaryVec;

SummaryVec read_summaries(
    std::ifstream& nasa_spk_file, std::optional<const FileRecord> hdr);

// The start_i and end_i pointers of a summary point to a block of element
// records. Each element record is a set of chebyshev coefficients along with
// the value of the mid point and range that set of coefficients are valid for.
//
// Type II elements only have coeffcients for position:
//    +------------------+
//    | MID              |
//    +------------------+
//    | RADIUS           |
//    +------------------+
//    | X  coefficients  |
//    +------------------+
//    | Y  coefficients  |
//    +------------------+
//    | Z  coefficients  |
//    +------------------+
//
// While Type III elements have an additional set of coefficients for velocity:
//
//    +------------------+
//    | MID              |
//    +------------------+
//    | RADIUS           |
//    +------------------+
//    | X  coefficients  |
//    +------------------+
//    | Y  coefficients  |
//    +------------------+
//    | Z  coefficients  |
//    +------------------+
//    | X' coefficients  |
//    +------------------+
//    | Y' coefficients  |
//    +------------------+
//    | Z' coefficients  |
//    +------------------+

Elements
read_elements(std::ifstream& nasa_spk_file, int coefficient_n, int data_type);

void read_coefficients(
    std::ifstream& nasa_spk_file, size_t n_coeff, std::vector<double>& V);

// At the end of the set of element records is a footer carrying some
// metadata for that block of element records
struct ElementRecordMetadata {
    double init;   // The start time (in s) of the epoch represented
    double intlen; // The length of the interval represented
    double rsize;  // The size of the record in units of 8 bytes (a double)
    double n;      // The number of records contained here
};

ElementRecordMetadata
read_element_record_metadata(std::ifstream& nasa_spk_file, const Summary& s);

Ephemeris read_ephemeris(
    std::ifstream& nasa_spk_file,
    const Summary& s,
    double         begin_jd,
    double         end_jd);

//
// Function defenitions follow /////////////////////////////////////////
//

EphemerisVec
load_spk(std::ifstream& nasa_spk_file, double begin_jd, double end_jd)
{

    auto hdr = read_file_record(nasa_spk_file);
    if (!hdr) {
        LOG_S(ERROR) << "Problem with header";
        return {};
    }

    // std::string comment = read_comment_blocks(nasa_spk_file, hdr);
    // std::cout << comment;

    EphemerisVec eph_vec;
    for (auto summary : read_summaries(nasa_spk_file, hdr)) {

        if (summary.frame_id != 1) {
            LOG_S(WARNING) << "Body: " << summary.target_id
                           << ": can only handle J2000 frame, this is "
                           << summary.frame_id << ". Skipping.";
            continue;
        }

        if ((jd_to_j2000(begin_jd) < summary.begin_second)
            || (jd_to_j2000(end_jd) > summary.end_second)) {
            LOG_S(WARNING) << "Body: " << summary.target_id
                           << ": Insufficient ephemeris data for simulation";
            continue;
        }

        if (!((summary.data_type == 2) || (summary.data_type == 3))) {
            LOG_S(WARNING) << "Body: " << summary.target_id
                           << ": Can only handle data type II or III. Skipping";
            continue;
        }

        eph_vec.push_back(EphShrPtr(new Ephemeris(
            read_ephemeris(nasa_spk_file, summary, begin_jd, end_jd))));
    }

    return eph_vec;
}

// Read the file header record
std::optional<FileRecord> read_file_record(std::ifstream& nasa_spk_file)
{
    FileRecord hdr;

    nasa_spk_file.seekg(0);
    nasa_spk_file.read((char*)&hdr, sizeof(hdr));

    LOG_S(INFO) << "File architecture: " << hdr.file_architecture;
    LOG_S(INFO) << "Internal name: " << hdr.internal_name;
    LOG_S(INFO) << "Numeric format: " << hdr.numeric_format;

    if ((hdr.n_double_precision != 2) & (hdr.n_integers != 6)) {
        LOG_S(ERROR) << "Expected ND = 2, NI = 6, but got "
                     << hdr.n_double_precision << ", " << hdr.n_integers;
        return {};
    }

    if (integrity_check_template.compare(hdr.integrity_string) != 0) {
        LOG_S(ERROR) << "This file is likely corrupted: FTP validation fails";
        return {};
    }

    LOG_S(INFO) << "Header looks fine";

    return hdr;
}

// The comment is stored in blocks of 1024 bytes, but only the first 1000 bytes
// of that are used. The null character (\0) is used instead of a new line while
// the \4 character indicates end of the comment. As a result, we need to read
// the comment as blocks of 1024 characters, discard the last 24, replace \0
// with \n and \4 with \0 at which point we have the whole comment and can stop.
// Considering we don't have any use for the comment string, this was really not
// necessary and done more out of a desire to be completionist.
std::string
read_comment_blocks(std::ifstream& nasa_spk_file, std::optional<FileRecord> hdr)
{
    if (!hdr) {
        return "";
    }

    std::string comment;
    nasa_spk_file.seekg(block_size);
    for (int i = 2; i < hdr->first_summary_block; i++) {
        char buf[block_size];
        nasa_spk_file.read(buf, block_size);
        for (int i = 0; i < 1000; i++) {
            if (buf[i] == '\0') {
                buf[i] = '\n';
            } else {
                if (buf[i] == '\4') {
                    buf[i] = '\0';
                    break;
                }
            }
        }
        comment += buf;
    }
    return comment;
}

SummaryVec read_summaries(
    std::ifstream& nasa_spk_file, std::optional<const FileRecord> hdr)
{
    SummaryRecordBlockHeader srbh;
    SummaryVec               sv;

    if (!hdr) {
        return sv;
    }

    size_t summaries_read = 0;

    nasa_spk_file.seekg(block_size * (hdr->first_summary_block - 1));
    nasa_spk_file.read((char*)&srbh, sizeof(srbh));

    for (;;) {
        LOG_S(INFO) << "Summary record block#" << ++summaries_read;
        for (size_t j = 0; j < srbh.n_summaries; j++) {
            Summary es;
            nasa_spk_file.read((char*)&es, sizeof(es));
            sv.push_back(es);
            LOG_S(INFO) << "Summary " << j + 1 << "/" << srbh.n_summaries;
        }

        if (srbh.next_summary_record_blk == 0)
            break;

        nasa_spk_file.seekg(block_size * (srbh.next_summary_record_blk - 1));
        nasa_spk_file.read((char*)&srbh, sizeof(srbh));
    }

    return sv;
}

Ephemeris read_ephemeris(
    std::ifstream& nasa_spk_file,
    const Summary& s,
    double         begin_jd,
    double         end_jd)
{
    double begin_s = jd_to_j2000(begin_jd);
    double end_s   = jd_to_j2000(end_jd);

    ElementRecordMetadata erm = read_element_record_metadata(nasa_spk_file, s);

    size_t begin_element = std::floor((begin_s - erm.init) / erm.intlen);
    size_t end_element   = std::floor((end_s - erm.init) / erm.intlen);

    size_t n_coeff;
    if (s.data_type == 2) {
        n_coeff = (int(erm.rsize) - 2) / 3; // Type II - has one sets of coeffs
    }
    if (s.data_type == 3) {
        n_coeff = (int(erm.rsize) - 2) / 6; // Type III - has two sets of coeffs
    }

    size_t internal_offset_byte
        = (begin_element * erm.rsize + s.start_i - 1) * size_of_double;
    nasa_spk_file.seekg(internal_offset_byte);

    Ephemeris eph;

    eph.target_code = s.target_id;
    eph.center_code = s.center_id;

    eph.evec.reserve(end_element - begin_element + 1);
    for (size_t i = begin_element; i <= end_element; i++) {
        eph.evec.push_back(read_elements(nasa_spk_file, n_coeff, s.data_type));
    }

    LOG_S(INFO) << "Ephemeris for " << s.target_id << ": " << n_coeff - 1
                << " order polynomial, " << eph.evec.size() << " elements, ";

    return eph;
}

// std::ostream& operator<<(std::ostream& out, const Summary& es)
// {
//     out << "(" << es.start_second << " - " << es.end_second
//         << ": T:" << es.target << " C:" << es.center << ")";
//     return out;
// }

// std::ostream& operator<<(std::ostream& out, const ElementRecordMetadata& erm)
// {
//     out << "(" << erm.init << ", " << erm.intlen << ", " << erm.rsize << ", "
//         << erm.n;
//     return out;
// }

// Element data are stored sequentially, so this function is meant to be called
// sequentially.
Elements read_elements(std::ifstream& nasa_spk_file, int n_coeff, int data_type)
{
    Elements elem;

    nasa_spk_file.read((char*)&elem.t_mid, sizeof(double));
    nasa_spk_file.read((char*)&elem.t_half, sizeof(double));

    read_coefficients(nasa_spk_file, n_coeff, elem.X);
    read_coefficients(nasa_spk_file, n_coeff, elem.Y);
    read_coefficients(nasa_spk_file, n_coeff, elem.Z);

    if (data_type == 3) { // Need to skip the unused coefficients
        nasa_spk_file.seekg(3 * n_coeff * 8, std::ios::cur);
    }

    return elem;
}

// Coefficients are stored sequentially.
void read_coefficients(
    std::ifstream& nasa_spk_file, size_t n_coeff, std::vector<double>& V)
{
    V.reserve(n_coeff);
    for (int j = 0; j < n_coeff; j++) {
        double c;
        nasa_spk_file.read((char*)&c, sizeof(double));
        V.push_back(c);
    }
}

ElementRecordMetadata
read_element_record_metadata(std::ifstream& nasa_spk_file, const Summary& s)
{
    ElementRecordMetadata erm;
    nasa_spk_file.seekg((s.end_i - 4) * 8);
    nasa_spk_file.read((char*)&erm, sizeof(erm));
    return erm;
}

/*
std::optional<ChebyVec> read_ephemeris_coefficients(
    std::ifstream& nasa_spk_file, const Summary& s, double start_s,
    double end_s)
{
    if (!((s.data_type == 2) || (s.data_type == 3))) {
        LOG_S(ERROR) << "Can only handle data type II or III";
        return {};
    }

    if (start_s < s.start_second) {
        LOG_S(ERROR) << "Start time out of range";
        return {};
    }

    if (end_s > s.end_second) {
        LOG_S(ERROR) << "End time out of range";
        return {};
    }

    ElementRecordMetadata erm = read_element_record_metadata(nasa_spk_file, s);

    size_t internal_offset_start
        = std::floor((start_s - erm.init) / erm.intlen) * erm.rsize;
    size_t internal_offset_end
        = std::floor((end_s - erm.init) / erm.intlen) * erm.rsize;
    size_t n_coeff = (int(erm.rsize) - 2) / (3 * (s.data_type - 1));

    ChebyVec cv;

    nasa_spk_file.seekg((s.start_i + internal_offset_start - 1) * 8);
    for (size_t i = internal_offset_start; i <= internal_offset_end; i++) {

        ChebyshevCoefficients cc;

        nasa_spk_file.read((char*)&cc.t_mid, sizeof(double));
        nasa_spk_file.read((char*)&cc.t_half, sizeof(double));

        read_coefficients(nasa_spk_file, n_coeff, cc.X);
        read_coefficients(nasa_spk_file, n_coeff, cc.Y);
        read_coefficients(nasa_spk_file, n_coeff, cc.Z);

        cv.push_back(cc);

        // if (s.data_type == 3) {
        //     nasa_spk_file.seekg(
        //         3 * n_coeff * 8, std::ios::relative); // TODO: XXX relative
        // }
    }
    return cv;
}
*/

struct ElementRecord {
    std::vector<double>   coefficients;
    Summary               es;
    ElementRecordMetadata erm;
};

ElementRecord read_element_record(std::ifstream& nasa_spk_file, Summary es)
{
    ElementRecord er;
    er.es = es;
    nasa_spk_file.seekg((es.end_i - 4) * 8);
    nasa_spk_file.read((char*)&er.erm, sizeof(er.erm));

    // std::vector<double> coefficients;
    // er.coefficients.resize((es.end_i - 4) - es.start_i);
    er.coefficients.resize(1000);

    nasa_spk_file.seekg((es.start_i - 1) * 8);
    nasa_spk_file.read(
        reinterpret_cast<char*>(er.coefficients.data()),
        er.coefficients.size() * sizeof(double));

    return er;
}

typedef std::vector<ElementRecord> ElementRecordVec;

/*
SummaryVec read_summaries(
    std::ifstream& nasa_spk_file, std::optional<const FileRecord> hdr)
{
    SummaryRecordBlockHeader sr;
    ElementRecordVec         erv;

    size_t summary_records_read = 0;

    nasa_spk_file.seekg(block_size * (segment_start_block - 1));
    nasa_spk_file.read((char*)&sr, sizeof(sr));

    for (;;) {
        LOG_S(INFO) << "Read summary record #" << ++summary_records_read;
        for (size_t n_element = 0; n_element < sr.n_element_records;
             n_element++) {
            Summary es;
            nasa_spk_file.read((char*)&es, sizeof(es));
            erv.push_back(read_element_record(nasa_spk_file, es));
            LOG_S(INFO) << "Read element summary " << n_element + 1 << "/"
                        << sr.n_element_records << " " << es;
        }

        if (sr.next_summary_record_blk == 0)
            break;

        nasa_spk_file.seekg(block_size * (sr.next_summary_record_blk - 1));
        nasa_spk_file.read((char*)&sr, sizeof(sr));
    }

    return erv;
}
*/
// size_t read_summary_record(std::ifstream& nasa_spk_file, size_t
// segment_start_block)
// {
//     char buf[block_size];
//     read_record_block(nasa_spk_file, segment_start_block, buf);
//     double next_segment_start_block = *(double*)&buf[0];
//     double n_blocks_in_segment      = *(double*)&buf[16];

//     return (size_t)next_segment_start_block;
// }
}