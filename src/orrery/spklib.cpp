/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2018-2020 by Kaushik Ghose. Some rights reserved, see LICENSE

Low level functions and data structures to load data from a SPK file
*/

#include "spklib.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

namespace groho {

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

sumry_map_t read_summaries(
    std::ifstream& nasa_spk_file, std::optional<const FileRecord> hdr)
{
    SummaryRecordBlockHeader srbh;
    sumry_map_t              sv;

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
            sv[es.target_id] = es;
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