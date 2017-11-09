/*
  Code to handle the common aspects of parsing scenario and flightplan files

  Every entry in an input file is of the pattern

  text: text
  number: text
  
  Comments: If a # is found anywhere the rest of that line is discarded

  Header: We expect all files to begin with the "text: text" pattern 

  Program: flight plan files have a second section that is all "number: text"

*/
#pragma once

#include <string>
#include <vector>
#include <istream>
#include <regex>

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"


namespace sim
{

struct Token
{
  std::string key, value;
};


enum class Expecting
{
  ExactlyOne,
  ZeroOrMore,
  OneOrMore
};

struct TokenizedScenario
{
  std::vector<Token> header, program;
  // main scenario files only have headers
  // flight plans have both a header and a program
  bool contains_errors;
  size_t line_with_error;
  std::string error_message;
  // We try not to crash on malformed inputs - that would be annoying
  // Instead we flag the scenario as corrupt and return an informative message


  std::vector<std::string> 
  get_header_value( std::string key, std::string def, Expecting exp = Expecting::ExactlyOne )
  {
    std::vector<std::string> values;
    for( auto& tk : header ) { if( tk.key == key ) values.push_back( tk.value ); }
    
    if( (values.size() == 0) & ( (exp == Expecting::ExactlyOne) | (exp == Expecting::OneOrMore) )) 
    {
      LOG_S(WARNING) << "Did not find \"" << key << "\": using default value " << def;
      values.push_back( def );
    }
    if( (values.size() > 1) & (exp == Expecting::ExactlyOne) ) 
    {
      LOG_S(WARNING) << "Found multiple lines with \"" << key << "\"";
    }

    return values;
  }
};


std::string 
trim( const std::string& in )
{
  size_t i0 = in.find_first_not_of(' '),
         i1 = in.find_last_not_of(' ');
  return in.substr(i0, i1 - i0 + 1);
}


std::regex number_pattern("-?[0-9]+.?[0-9]+");

std::istream& 
operator >> ( std::istream& scn_file, TokenizedScenario& ts )
{
  size_t idx, line_ctr = 0;
  std::string line;

  while( std::getline( scn_file, line ) )  
  {
    line_ctr++;
    // Chomp comment
    idx = line.find_first_of("#");
    if( idx != -1 ) { line = line.substr(0, idx); }
    // Ignore functionally empty lines
    if( line.length() == 0 ) continue;

    idx = line.find_first_of(":");
    if( idx == -1 ) continue;
    
    Token t;
    t.key = trim( line.substr(0, idx) );
    t.value = trim( line.substr(idx + 1, -1) );
    
    if( (t.key.length() == 0) | (t.value.length() == 0) )
    {
      ts.contains_errors = true;
      ts.line_with_error = line_ctr;
      ts.error_message = "Key or value missing on line " + std::to_string( line_ctr );
      break;
    }

    bool key_is_number = std::regex_match( t.key, number_pattern);
    if( key_is_number ) { ts.program.push_back( t ); }
                   else {  ts.header.push_back( t ); }

    //DLOG_S(INFO) << "(" << t.key << "), (" << t.value << ")";
  }

  return scn_file;
}


} // namespace sim


