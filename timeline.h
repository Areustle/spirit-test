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
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include <vector>

namespace client
{
	namespace fusion = boost::fusion;
	namespace phoenix = boost::phoenix;
	namespace qi = boost::spirit::qi;
	namespace ascii = boost::spirit::ascii;

	struct rockprofile_pair
	{
		int rocktime;
		double rockangle;
	};

	struct rocking_profile
	{
		std::string rockstart;
		int rockstart_met;
		double rockdefault;
		std::vector<rockprofile_pair> pairs;
	};

	struct opt_evt_fields
	{
		std::string prop_ID;
		std::string target_name;
		double offset;
		double RA;
		double dupRA;
		double DEC;
		double dupDEC;
		std::string PI;
		std::string comment;
		int week;
		int SSN;
		double duration;
		int slew;
		int saa;
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

	struct initial{
		int week;
		std::string timeline_name;
		std::string create_time;
		std::string creator;
		double RA;
		double DEC;
		rocking_profile profile;
		timeline_event event;
		std::vector<std::string> tako_db;
		std::vector<std::string> sc_ephem;
		std::vector<std::string> saa;
		std::vector<std::string> tdrss_ephem;
		std::vector<std::string> tdrss_sched;
		std::string prev_arr_thresh;
	};

	struct timeline_header
	{
		std::string filename;
		std::string creation_time;
		std::string mission_id;
		std::string originator;
		std::string db_version;
		std::string dest_processor;
		std::string start_time;
		std::string stop_time;
		std::string execute_flag;
		std::string timeline_type;
		std::string version_num;
		std::string ref_timeline_name;
		std::string comment;
	};

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
		timeline_header header;
		initial init;
		//std::vector<command> commands;
		std::vector<timeline_event> events;
	};
}

BOOST_FUSION_ADAPT_STRUCT(
		client::rockprofile_pair,
		(int, rocktime)
		(double, rockangle)
		)

BOOST_FUSION_ADAPT_STRUCT(
		client::rocking_profile,
		(std::string, rockstart)
		(int, rockstart_met)
		(double, rockdefault)
		(std::vector<client::rockprofile_pair>, pairs)
		)

BOOST_FUSION_ADAPT_STRUCT(
		client::opt_evt_fields,
		(std::string, prop_ID)
		(std::string, target_name)
		(std::string, PI)
		(std::string, comment)
		(double, offset)
		(double, RA)
		(double, dupRA)
		(double, DEC)
		(double, dupDEC)
		(int, week)
		(int, SSN)
		(double, duration)
		(int, slew)
		(int, saa)
		(client::rocking_profile, profile)
		)

BOOST_FUSION_ADAPT_STRUCT(
		client::initial,
		(int, week)
		(std::string, timeline_name)
		(std::string, create_time)
		(std::string, creator)
		(double, RA)
		(double, DEC)
		(client::rocking_profile, profile)
		(client::timeline_event, event)
		(std::vector<std::string>, tako_db)
		(std::vector<std::string>, sc_ephem)
		(std::vector<std::string>, saa)
		(std::vector<std::string>, tdrss_ephem)
		(std::vector<std::string>, tdrss_sched)
		(std::string, prev_arr_thresh)
		)
	/* initial %= */


BOOST_FUSION_ADAPT_STRUCT(
		client::timeline_event,
		(std::string, timestamp)
		(std::string, event_name)
		(std::string, event_type)
		(std::string, obs_number)
		(client::opt_evt_fields, additional)
		)

BOOST_FUSION_ADAPT_STRUCT(
		client::command,
		(std::string, time)
		(std::string, type)
		(std::string, order)
		//(std::vector<std::string>,  params)
		(std::string, params)
		)


BOOST_FUSION_ADAPT_STRUCT(
		client::timeline_header,
		(std::string, filename)
		(std::string, creation_time)
		(std::string, mission_id)
		(std::string, originator)
		(std::string, db_version)
		(std::string, dest_processor)
		(std::string, start_time)
		(std::string, stop_time)
		(std::string, execute_flag)
		(std::string, timeline_type)
		(std::string, version_num)
		(std::string, ref_timeline_name)
		(std::string, comment)
		)

BOOST_FUSION_ADAPT_STRUCT(
		client::timeline_wrapper,
		(client::timeline_header, header)
		(client::initial, init)
		//(std::vector<client::command>, commands)
		(std::vector<client::timeline_event>, events)
		)
	//]

namespace client {

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
			using ascii::space;
			using ascii::string;
			using qi::alnum;
			using qi::blank;


			file_path %=
				lexeme[ +( -char_("/") >> +(qi::graph - char_("/") - char_(".")) )
				>> char_(".") >> +qi::alnum ];


			divider = lit("//") >> lexeme[+char_("-") >> qi::eol];


			timestamp %=
				repeat(4)[digit] >> char_("/")
				>> repeat(3)[digit]
				>> repeat(3)[ char_(":") >> qi::repeat(2)[digit] ]
				;

			one_liner %= lexeme[+(char_ - qi::eol)];

			rocktime_angle %= lit("//") >> qi::omit[digit >> digit] >> int_ >> double_ ;

			rocking_profile %=
				lit("//") >> lit("Rocking Profile:")
				>> lit("//")>>lit("ROCKSTART") >> "=" >> timestamp >> '(' >> int_ >> ')'
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


			opt_evt_fields %=
				(lit("//") >> lit("prop_ID") >> "=" >> +digit)
				^ (lit("//") >> lit("target_name") >> "=" >> one_liner)
				^ (lit("//") >> lit("PI") >> "=" >> one_liner)
				^ (lit("//") >> lit("comment") >> "=" >> one_liner)
				^ (lit("//") >> lit("offset") >> "=" >> qi::double_ >> lit("deg"))
				^ (lit("//") >> lit("week") >> "=" >> qi::int_)
				^ (lit("//") >> lit("SSN") >> "=" >> qi::int_)
				^ (lit("//") >> lit("duration") >> "=" >> qi::double_ >> lit("ksec"))
				^ (lit("//") >> lit("slew") >> "=" >> qi::int_ >> lit("sec"))
				^ (lit("//") >> lit("saa") >> "=" >> qi::int_ >> lit("sec"))


				// Why are these duplicated?
				// The standard allows events optional fields to hold two RA and DEC
				// values. While they must be equivalent, they are both maintained for
				// some reason in Tako Timelines. So For ease I've just crated additional
				// fields to hold them, the dupRA and dupDEC fields.
				^ RA
				^ RA
				^ DEC
				^ DEC
				^ rocking_profile
				;


			file_name %=
				+(alnum | char_("_") | char_("."))
				;

			event %=
				lit("//") >> timestamp
				>> (string("Survey") | string("Obs") | string("Profile"))
				>> (string("Begin") | string("End") )
				>> (lit("obs_number") >> "=" >> lexeme[ string("Global") | +(digit | char_("-")) ])
				>> -opt_evt_fields
				;


			initial %=
				lit("//") >> "Mission Week:" >> int_
				>> "//" >> "Timeline Name:" >> lexeme[+(qi::graph)]
				>> lit("Created:") >> timestamp
				>> "//" >> "Created with" >> one_liner
				>> "//" >> "Initial pointing" >> RA >> DEC
				>> "//" >> "Initial survey-related flight parameter settings"
				>> rocking_profile
				>> -event
				>> "//"
				>> "//" >> "Input files:"
				>> "//" >> "TAKO database:" >> file_path % lit("//")
				>> "//" >> "Spacecraft ephemeris:" >> file_path % lit("//")
				>> "//" >> "SAA:" >> file_path % lit("//")
				>> "//" >> "TDRSS ephemeris:" >> file_path % lit("//")
				>> "//" >> "TDRSS contact schedule:" >> file_path % lit("//")
				>> "//"
				>> "//" >> "Previous ARR Threshold was" >> *qi::alpha
				;

			command_param %= "("
				>> lexeme[+(char_ - char_(")"))]
				>> ")"
				;


			command %=
				timestamp
				>> (string("CMD") | string("ACT"))
				>> lexeme[+alnum]
				>> -command_param
				>>";"
				;


			header %=
				file_name													// File Name
				>> "," >> timestamp								// Creation Time
				>> "," >> *alnum									// Mission Identifier
				>> "," >> lexeme[*(alnum)]				// Originator
				>> "," >> *(alnum | char_("."))		// Project Database Version
				>> "," >> *alnum									// Destination processor
				>> "," >> timestamp								// Start Time
				>> "," >> timestamp								// Stop Time
				>> "," >> *alnum									// Execute Flag
				>> "," >> *alnum									// Timeline type
				>> "," >> qi::repeat(2)[digit]		// Version number
				>> "," >> file_name								// Reference Timeline Filename
				>> "," >> lexeme[+(char_ - ";")]	// Comment
				>> ";"
				;

			generic_comment %= !(event | initial)
				>> lit("//")
				>> one_liner
				;

			ignored %= divider | command | generic_comment;

			timeline %= *ignored
				>> -header >> *ignored
				>> -initial >> *ignored
				>> *(event | ignored) >> *ignored
				>> qi::eoi
				;
		}

		// Start Rule for timelines
		qi::rule<Iterator, timeline_wrapper(), ascii::space_type> timeline;

		// Rules to parse compnents of a timeline file into their various
		// data structures needed for the timeline object.
		qi::rule<Iterator, timeline_header(), ascii::space_type> header;
		qi::rule<Iterator, initial(), ascii::space_type> initial;
		qi::rule<Iterator, timeline_event(), ascii::space_type> event;
		qi::rule<Iterator, opt_evt_fields(), ascii::space_type> opt_evt_fields;
		qi::rule<Iterator, rocking_profile(), ascii::space_type> rocking_profile;
		qi::rule<Iterator, rockprofile_pair(), ascii::space_type> rocktime_angle;

		// Sub-parsers
		typedef qi::rule<Iterator, std::string(), ascii::space_type> string_rule;
		typedef qi::rule<Iterator, int, ascii::space_type> int_rule;
		typedef qi::rule<Iterator, double, ascii::space_type> double_rule;

		string_rule one_liner;
		string_rule file_path;
		string_rule command_param;

		string_rule timestamp;
		string_rule file_name;

		double_rule RA;
		double_rule DEC;

		// Un-needed strings and components of the Timeline file
		qi::rule<Iterator, void(), ascii::space_type> ignored;
		qi::rule<Iterator, void(), ascii::space_type> divider;
		qi::rule<Iterator, void(), ascii::space_type> generic_comment;
		qi::rule<Iterator, void(), ascii::space_type> command;

	};
	//]
}

class timeline
{
	public:
	timeline(char const* filename)
	{
		std::ifstream in(filename, std::ios_base::in);

		if (!in)
			std::cerr << "Error: Could not open input timeline file: "
				<< filename << std::endl;

		std::string storage; // We will read the contents here.
		in.unsetf(std::ios::skipws); // No white space skipping!
		std::copy(
				std::istream_iterator<char>(in),
				std::istream_iterator<char>(),
				std::back_inserter(storage));

		client::timeline_grammar<std::string::const_iterator> grammar;
		client::timeline_wrapper tlw;

		using boost::spirit::ascii::space;
		std::string::const_iterator iter = storage.begin();
		std::string::const_iterator end = storage.end();

		this->success = phrase_parse(iter, end, grammar, space, tlw);
		this->header = tlw.header;
		this->init = tlw.init;
		this->events = tlw.events;

	}

	timeline(std::string filename) { timeline( filename.c_str() ); }

	bool success;
	client::timeline_header header;
	client::initial init;
	std::vector<client::timeline_event> events;

};

#endif /* TIMELINE_H */
