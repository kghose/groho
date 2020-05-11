/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Low level functions and data structures to load data from a SPK file
*/

#pragma once

#include <fstream>
#include <optional>
#include <stdlib.h>

#include "spk.hpp"

namespace groho {

// In the following section all structures that closely reflect some part of
// the SPK/DAF file structure are named adhering to the nomenclature
// JPL/NASA adopted.

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

sumry_map_t read_summaries(
    std::ifstream& nasa_spk_file, std::optional<const FileRecord> hdr);

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

}