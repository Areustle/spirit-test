#ifndef TIMELINE_H
#define TIMELINE_H

/*
 * =====================================================================================
 *
 *       Filename:  timeline.h
 *
 *    Description:  A parsed timeline class for Fermi Timeline files.
 *		              This class is intended to be used with the Fermi gtorbsim
 *		              Orbit Simulator. It will parse properly formatted timeline
 *		              files into a timeline object whose callable fields conform
 *		              to elements of the parsed timeline.
 *		              It depends on the BOOST::SPIRIT parsing library.
 *
 *        Version:  1.0
 *        Created:  10/17/2017 15:48:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alex Reustle, Joe Asercion
 *   Organization:  Fermi Science Support Center
 *
 * =====================================================================================
 */

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include <vector>

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;

struct rockprofile_pair
{
  double rocktime;
  double rockangle;
};

struct rocking_profile
{
  std::string rockstart;
  double rockstart_met;
  double rockdefault;
  std::vector<rockprofile_pair> pairs;
};

struct opt_evt_fields
{
  std::string prop_ID;
  std::string target_name;
  double offset;
  double RA;
  double DEC;
  std::string PI;
  std::string comment;
  int week;
  int SSN;
  double duration;
  int slew;
  int saa;
  double dupRA;
  double dupDEC;
  rocking_profile profile;
};

struct timeline_event
{
  std::string timestamp;
  std::string event_name;
  std::string event_type;
  std::string obs_number;

  //Optional fields
  opt_evt_fields additional;
};

struct timeline_initial{
  int week;
  std::string timeline_name;
  std::string create_time;
  std::string creator;
  double RA;
  double DEC;
  rocking_profile profile;
  timeline_event event;
  /* std::vector<std::string> tako_db; */
  /* std::vector<std::string> sc_ephem; */
  /* std::vector<std::string> saa; */
  /* std::vector<std::string> tdrss_ephem; */
  /* std::vector<std::string> tdrss_sched; */
  /* std::string prev_arr_thresh; */
};

/* struct timeline_header */
/* { */
/*   std::string filename; */
/*   std::string creation_time; */
/*   std::string mission_id; */
/*   std::string originator; */
/*   std::string db_version; */
/*   std::string dest_processor; */
/*   std::string start_time; */
/*   std::string stop_time; */
/*   std::string execute_flag; */
/*   std::string timeline_type; */
/*   std::string version_num; */
/*   std::string ref_timeline_name; */
/*   std::string comment; */
/* }; */

struct command
{
  std::string time;
  std::string type;
  std::string order;
  //std::vector<std::string> params;
  std::string params;
};

struct timeline_wrapper
{
  /* timeline_header header; */
  std::string header;
  timeline_initial init;
  //std::vector<command> commands;
  std::vector<timeline_event> events;
};

BOOST_FUSION_ADAPT_STRUCT(
    rockprofile_pair,
    (double, rocktime)
    (double, rockangle)
    )

BOOST_FUSION_ADAPT_STRUCT(
    rocking_profile,
    (std::string, rockstart)
    (double, rockstart_met)
    (double, rockdefault)
    (std::vector<rockprofile_pair>, pairs)
    )

BOOST_FUSION_ADAPT_STRUCT(
    opt_evt_fields,
    (std::string, prop_ID)
    (std::string, target_name)
    (double, offset)
    (double, RA)
    (double, DEC)
    (std::string, PI)
    (std::string, comment)
    (int, week)
    (int, SSN)
    (double, duration)
    (int, slew)
    (int, saa)
    (double, dupRA)
    (double, dupDEC)
    (rocking_profile, profile)
    )

BOOST_FUSION_ADAPT_STRUCT(
    timeline_initial,
    (int, week)
    (std::string, timeline_name)
    (std::string, create_time)
    (std::string, creator)
    (double, RA)
    (double, DEC)
    (rocking_profile, profile)
    (timeline_event, event)
    /* (std::vector<std::string>, tako_db) */
    /* (std::vector<std::string>, sc_ephem) */
    /* (std::vector<std::string>, saa) */
    /* /1* (std::vector<std::string>, tdrss_ephem) *1/ */
    /* /1* (std::vector<std::string>, tdrss_sched) *1/ */
    /* /1* (std::string, prev_arr_thresh) *1/ */
    )


BOOST_FUSION_ADAPT_STRUCT(
    timeline_event,
    (std::string, timestamp)
    (std::string, event_name)
    (std::string, event_type)
    (std::string, obs_number)
    (opt_evt_fields, additional)
    )

BOOST_FUSION_ADAPT_STRUCT(
    command,
    (std::string, time)
    (std::string, type)
    (std::string, order)
    //(std::vector<std::string>,  params)
    (std::string, params)
    )


/* BOOST_FUSION_ADAPT_STRUCT( */
/*     timeline_header, */
/*     (std::string, filename) */
/*     (std::string, creation_time) */
/*     (std::string, mission_id) */
/*     (std::string, originator) */
/*     (std::string, db_version) */
/*     (std::string, dest_processor) */
/*     (std::string, start_time) */
/*     (std::string, stop_time) */
/*     (std::string, execute_flag) */
/*     (std::string, timeline_type) */
/*     (std::string, version_num) */
/*     (std::string, ref_timeline_name) */
/*     (std::string, comment) */
/*     ) */

BOOST_FUSION_ADAPT_STRUCT(
    timeline_wrapper,
    (std::string, header)
    (timeline_initial, init)
    //(std::vector<command>, commands)
    (std::vector<timeline_event>, events)
    )
  //]


  template <typename Iterator>
    struct timeline_grammar : qi::grammar<Iterator, timeline_wrapper(), ascii::space_type>
  {
    timeline_grammar() : timeline_grammar::base_type(timeline)
    {
      using qi::int_;
      using qi::lit;
      using qi::double_;
      using qi::lexeme;
      using qi::repeat;
      using ascii::char_;
      using ascii::digit;
      using ascii::string;
      using qi::alnum;
      using boost::spirit::qi::_1;
      using boost::spirit::qi::_val;
      using boost::phoenix::at_c;


      divider %= lit("//") >> lexeme[+char_("-") >> qi::eol];

      timestamp %=
        repeat(4)[digit] >> char_("/")
        >> repeat(3)[digit]
        >> repeat(3)[ char_(":") >> qi::repeat(2)[digit] ]
        ;

      one_liner %= lexeme[+(char_ - qi::eol)];

      rocktime_angle %= lit("//") >> qi::omit[digit >> digit] >> double_ >> double_ ;

      tl_rocking_profile %=
        lit("//") >> lit("Rocking Profile:")
        >> lit("//")>>lit("ROCKSTART") >> "=" >> timestamp >> '(' >> double_ >> ')'
        >> lit("//") >> lit("ROCKDEFAULT") >> "=" >> double_
        >> lit("//") >> lit("ROCKTIME") >> lit("ROCKANGLE")
        >> repeat(17)[rocktime_angle]
        ;

      RA %=
        lit("//")
        >> lit("RA")
        >> "="
        >> qi::double_
        >> -lit("deg")
        ;


      DEC %=
        lit("//")
        >> qi::omit[ qi::no_case["DEC"] ]
        >> "="
        >> qi::double_
        >> -lit("deg")
        ;


      tl_opt_evt_fields =
        (lit("//") >> lit("prop_ID") >> "=" >> one_liner) [at_c<0>(_val) = _1]
        ^ (lit("//") >> lit("target_name") >> "=" >> one_liner) [at_c<1>(_val) = _1]
        ^ (lit("//") >> lit("offset") >> "=" >> qi::double_ >> lit("deg")) [at_c<2>(_val) = _1]
        ^ RA    [at_c<3>(_val) = _1]
        ^ DEC   [at_c<4>(_val) = _1]
        ^ (lit("//") >> lit("PI") >> "=" >> one_liner) [at_c<5>(_val) = _1]
        ^ (lit("//") >> lit("comment") >> "=" >> one_liner[at_c<6>(_val) = _1])
        ^ (lit("//") >> lit("week") >> "=" >> qi::int_[at_c<7>(_val) = _1])
        ^ (lit("//") >> lit("SSN") >> "=" >> qi::int_[at_c<8>(_val) = _1])
        ^ (lit("//") >> lit("duration") >> "=" >> qi::double_[at_c<9>(_val) = _1] >> lit("ksec"))
        ^ (lit("//") >> lit("slew") >> "=" >> qi::int_[at_c<10>(_val) = _1] >> lit("sec"))
        ^ (lit("//") >> lit("saa") >> "=" >> qi::int_[at_c<11>(_val) = _1] >> lit("sec"))

        // Why are these duplicated?
        // The standard allows events optional fields to hold two RA and DEC
        // values. While they must be equivalent, they are both maintained for
        // some reason in Tako Timelines. So For ease I've just crated additional
        // fields to hold them, the dupRA and dupDEC fields.
        ^ RA[at_c<12>(_val) = _1]
        ^ DEC[at_c<13>(_val) = _1]
        ^ tl_rocking_profile[at_c<14>(_val) = _1]
        ;


      file_name %= +(alnum | char_("_") | char_(".")) ;

      file_path %= +(char_("/") >> file_name) ;

      file_list %= file_path % lit("//");
      file_line %= file_path >> lit("//");

      event %=
        lit("//") >> timestamp
        >> (string("Survey") | string("Obs") | string("Profile"))
        >> (string("Begin") | string("End") )
        >> -(lit("obs_number") >> "=" >> lexeme[ string("Global") | +(digit | char_("-")) ])
        >> -tl_opt_evt_fields
        ;


      tl_initial %=
        lit("//") >> "Mission Week:" >> int_
        >> "//" >> "Timeline Name:" >> file_name
        >> lit("Created:") >> timestamp
        >> "//" >> "Created with" >> one_liner
        >> "//" >> "Initial pointing"
        >> RA
        >> DEC
        >> "//" >> "Initial survey-related flight parameter settings"
        >> tl_rocking_profile
        >> -event
        >> "//"
        >> "// Input files:"
        // I do not know how to parse these lists of file paths into vectors
        // of strings. If necessary I will try again later. For now
        // I'll just match everything up to empty line.
        /* >> "// TAKO database:" >> */
        /* *(-lit("//") >> file_line [phoenix::push_back(at_c<8>(_val), _1)]) */
        /* >> "// Spacecraft ephemeris:" >> */
        /* *(-lit("//") >> file_line [phoenix::push_back(at_c<9>(_val), _1)]) */
        /* >> "// SAA:" >> */
        /* *(-lit("//") >> file_line [phoenix::push_back(at_c<10>(_val), _1)]) */
        /* >> "// TDRSS ephemeris:" >> file_list [at_c<11>(_val) = _1] */
        /* >> "// TDRSS contact schedule:" >> file_list [at_c<12>(_val) = _1] */
        /* >> "//" */
        /* >> "//" >> "Previous ARR Threshold was" >> *qi::alpha [at_c<8>(_val) = _1] */

        // This should match everything up to an empty line.
        >> qi::eol >> qi::eol
        ;


      command_param %= "(" >> lexeme[+(char_ - char_(")"))] >> ")" ;

      command %=
        timestamp
        >> (string("CMD") | string("ACT"))
        >> lexeme[+alnum]
        >> -command_param
        >>";"
        ;

      header %= *(char_ - ";") >> ";";

      /* header %= file_name                 // File Name */
      /*   >> "," >> timestamp               // Creation Time */
      /*   >> "," >> *alnum                  // Mission Identifier */
      /*   >> "," >> *(alnum)                // Originator */
      /*   >> "," >> *(alnum | char_("."))   // Project Database Version */
      /*   >> "," >> *alnum                  // Destination processor */
      /*   >> "," >> timestamp               // Start Time */
      /*   >> "," >> timestamp               // Stop Time */
      /*   >> "," >> *alnum                  // Execute Flag */
      /*   >> "," >> *alnum                  // Timeline type */
      /*   /1* >> "," >> *alnum                  // Version number *1/ */
      /*   /1* >> "," >> file_name               // Reference Timeline Filename *1/ */
      /*   >> "," >> lexeme[+(char_ - ";")]  // Comment */
      /*   >> ";" */
      /*   ; */

      generic_comment %= !(event | tl_initial)
        >> lit("//")
        >> one_liner
        ;

      ignored %= divider | command | generic_comment;

      timeline = *ignored
        >> -header [at_c<0>(_val) = _1]
        >> *ignored
        >> -tl_initial [at_c<1>(_val) = _1]
        >> *ignored
        >> *(event [phoenix::push_back(at_c<2>(_val), _1)]
            | ignored) >> *ignored
        >> qi::eoi
        ;
    }

    // Start Rule for timelines
    qi::rule<Iterator, timeline_wrapper(), ascii::space_type> timeline;

    // Rules to parse compnents of a timeline file into their various
    // data structures needed for the timeline object.
    qi::rule<Iterator, std::string(), ascii::space_type> header;
    qi::rule<Iterator, timeline_initial(), ascii::space_type> tl_initial;
    qi::rule<Iterator, timeline_event(), ascii::space_type> event;
    qi::rule<Iterator, opt_evt_fields(), ascii::space_type> tl_opt_evt_fields;
    qi::rule<Iterator, rocking_profile(), ascii::space_type> tl_rocking_profile;
    qi::rule<Iterator, rockprofile_pair(), ascii::space_type> rocktime_angle;

    // Sub-parsers
    typedef qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> str_vec;
    typedef qi::rule<Iterator, std::string(), ascii::space_type> string_rule;
    typedef qi::rule<Iterator, int, ascii::space_type> int_rule;
    typedef qi::rule<Iterator, double, ascii::space_type> double_rule;

    str_vec file_list;

    string_rule one_liner;
    string_rule file_path;
    string_rule file_line;
    string_rule file_name;
    string_rule command_param;
    string_rule timestamp;

    double_rule RA;
    double_rule DEC;

    // Un-needed strings and components of the Timeline file
    qi::rule<Iterator, void(), ascii::space_type> ignored;
    qi::rule<Iterator, void(), ascii::space_type> divider;
    qi::rule<Iterator, void(), ascii::space_type> generic_comment;
    qi::rule<Iterator, void(), ascii::space_type> command;

  };
  //]

class Timeline
{
public:
  /* Constructors */
  Timeline();
  Timeline(char const* filename);
  Timeline(std::string filename);

  void populate(char const* filename);
  void populate(std::string filename);

  // Member Variables
  bool success;
  std::string header;
  timeline_initial init;
  std::vector<timeline_event> events;
};

#endif /* TIMELINE_H */
