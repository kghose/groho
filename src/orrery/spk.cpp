/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

The following code handles loading of NASA/JPL SPK/DAF files that carry
information for planetary ephemerides.
*/

// TODO: The code as written pays no attention to cache locality. We should do a
// benchmark with a heavyweight kernel (say DE430) where we just compute the
// orrery and then rewrite the code to achieve better cache locality and see how
// much we improve things.

#include <cmath>
#include <fstream>

#include "spk.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

void Elements::read(std::ifstream& nasa_spk_file, int n_coeff, int data_type)
{
    nasa_spk_file.read((char*)&t_mid, sizeof(double));
    nasa_spk_file.read((char*)&t_half, sizeof(double));

    A.resize(3 * n_coeff);
    nasa_spk_file.read((char*)A.data(), 3 * n_coeff * sizeof(double));
    off1 = n_coeff;
    off2 = 2 * n_coeff;
    off3 = 3 * n_coeff;

    if (data_type == 3) { // Need to skip the unused coefficients
        nasa_spk_file.seekg(3 * n_coeff * 8, std::ios::cur);
    }
}

// https://en.wikipedia.org/wiki/Clenshaw_algorithm#Special_case_for_Chebyshev_series
inline double Elements::cheby_eval_one(double t, size_t i0, size_t i1) const
{
    double x  = (t - t_mid) / t_half;
    double x2 = 2 * x;
    double Tn, Tn_1 = x, Tn_2 = 1.0;
    double b = A[i0] * Tn_2 + A[i0 + 1] * Tn_1;
    for (size_t i = i0 + 2; i < i1; i++) {
        Tn = x2 * Tn_1 - Tn_2;
        b += Tn * A[i];
        Tn_2 = Tn_1;
        Tn_1 = Tn;
    }
    return b;
}

inline void Ephemeris::eval(double t, V3d& pos)
{
    const auto& element = elements[std::floor((t - begin_s) / interval_s)];

    pos.x = element.cheby_eval_one(t, 0, element.off1);
    pos.y = element.cheby_eval_one(t, element.off1, element.off2);
    pos.z = element.cheby_eval_one(t, element.off2, element.off3);
}

// In the following section all structures that closely reflect some part of
// the SPK/DAF file structure are named adhering to the nomenclature
// JPL/NASA adopted. The final structure (Emphemeris) which collates and
// organizes the data in a manner suitable for this program is named to
// match it's logical function.

//  Thanks go to JPL/NASA NAIF docs
//  ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/daf.html

//  the jplemphem Python code by Brandon Rhodes
//  https://github.com/brandon-rhodes/python-jplephem

//  And this stackoverflow answer
//  https://space.stackexchange.com/a/20715
//  to this question
//  https://space.stackexchange.com/questions/12506/what-is-the-exact-format-of-the-jpl-ephemeris-files

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

sumry_vec_t read_summaries(
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

std::optional<SpkFile> SpkFile::load(std::string file_name)
{
    std::ifstream nasa_spk_file(file_name, std::ios::binary);

    auto hdr = read_file_record(nasa_spk_file);
    if (!hdr) {
        LOG_S(ERROR) << file_name;
        LOG_S(ERROR) << "Not loading because of errors.";
        return {};
    }

    SpkFile spk_file;
    spk_file.file_name = file_name;
    spk_file.comment   = read_comment_blocks(nasa_spk_file, hdr);
    spk_file.summaries = read_summaries(nasa_spk_file, hdr);

    return spk_file;
}

//
// Function definitions follow /////////////////////////////////////////
//

// Read the file header record
std::optional<FileRecord> read_file_record(std::ifstream& nasa_spk_file)
{
    FileRecord hdr;

    nasa_spk_file.seekg(0);
    nasa_spk_file.read((char*)&hdr, sizeof(hdr));

    if (integrity_check_template.compare(hdr.integrity_string) != 0) {
        LOG_S(ERROR) << "Header integrity check fail.";
        return {};
    }

    DLOG_S(INFO) << "File architecture: " << hdr.file_architecture;
    DLOG_S(INFO) << "Internal name: " << hdr.internal_name;
    DLOG_S(INFO) << "Numeric format: " << hdr.numeric_format;

    if ((hdr.n_double_precision != 2) & (hdr.n_integers != 6)) {
        LOG_S(ERROR) << "Expected ND = 2, NI = 6, but got "
                     << hdr.n_double_precision << ", " << hdr.n_integers;
        return {};
    }

    DLOG_S(INFO) << "Header looks fine";

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
    for (size_t i = 2; i < hdr->first_summary_block; i++) {
        char buf[block_size];
        nasa_spk_file.read(buf, block_size);
        for (size_t j = 0; j < 1000; j++) {
            if (buf[j] == '\0') {
                buf[j] = '\n';
            } else {
                if (buf[j] == '\4') {
                    buf[j] = '\0';
                    break;
                }
            }
        }
        comment += buf;
    }

    const auto strEnd   = comment.find_last_not_of(" \n");
    const auto strRange = strEnd + 1;

    return comment.substr(0, strRange);
}

sumry_vec_t read_summaries(
    std::ifstream& nasa_spk_file, std::optional<const FileRecord> hdr)
{
    SummaryRecordBlockHeader srbh;
    sumry_vec_t              sv;

    if (!hdr) {
        return sv;
    }

    size_t summaries_read = 0;

    nasa_spk_file.seekg(block_size * (hdr->first_summary_block - 1));
    nasa_spk_file.read((char*)&srbh, sizeof(srbh));

    for (;;) {
        DLOG_S(INFO) << "Summary record block#" << ++summaries_read;
        for (size_t j = 0; j < srbh.n_summaries; j++) {
            Summary es;
            nasa_spk_file.read((char*)&es, sizeof(es));
            sv.push_back(es);
            DLOG_S(INFO) << "Summary " << j + 1 << "/" << srbh.n_summaries;
        }

        if (srbh.next_summary_record_blk == 0)
            break;

        nasa_spk_file.seekg(block_size * (srbh.next_summary_record_blk - 1));
        nasa_spk_file.read((char*)&srbh, sizeof(srbh));
    }

    return sv;
}

ElementRecordMetadata
read_element_record_metadata(std::ifstream& nasa_spk_file, const Summary& s)
{
    ElementRecordMetadata erm;
    nasa_spk_file.seekg((s.end_i - 4) * 8);
    nasa_spk_file.read((char*)&erm, sizeof(erm));
    return erm;
}

}