/*
 * =====================================================================================
 *
 *       Filename:  timeline.cxx
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/01/2017 14:56:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include "orbitSim/timeline.h"
#include <stdlib.h>

Timeline::Timeline(){};

Timeline::Timeline(char const* filename)
{
  Timeline::populate(filename);
}

Timeline::Timeline(std::string filename)
{
  Timeline::Timeline(filename.c_str());
}

void
Timeline::populate(char const* filename)
{
  std::ifstream in(filename, std::ios_base::in);

  if (!in)
    std::cerr << "Error: Could not open input timeline file: " << filename
              << std::endl;

  std::string storage;         // We will read the contents here.
  in.unsetf(std::ios::skipws); // No white space skipping!
  std::copy(std::istream_iterator<char>(in),
    std::istream_iterator<char>(),
    std::back_inserter(storage));

  timeline_grammar<std::string::const_iterator> grammar;
  timeline_wrapper tlw;

  using boost::spirit::ascii::space;
  std::string::const_iterator iter = storage.begin();
  std::string::const_iterator end = storage.end();

  this->success = phrase_parse(iter, end, grammar, space, tlw);
  this->header = tlw.header;
  this->init = tlw.init;
  this->events = tlw.events;
}

void
Timeline::populate(std::string filename)
{
  Timeline::populate(filename.c_str());
}
