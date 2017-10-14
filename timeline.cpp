/*=============================================================================
	Copyright (c) 2002-2010 Joel de Guzman

	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A parser for arbitrary tuples. This example presents a parser
//  for an timeline structure.
//
//  [ JDG May 9, 2007 ]
//
///////////////////////////////////////////////////////////////////////////////

/* #include <boost/config/warning_disable.hpp> */
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

	struct input_files{
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

	struct timeline
	{
		std::vector<timeline_event> events;
		initial init;
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
	client::timeline,
	(client::initial, init)
	(std::vector<client::timeline_event>, events)
)
//]

namespace client
{
	template <typename Iterator>
	struct timeline_grammar
		: qi::grammar<Iterator, timeline(), ascii::space_type>
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

			timestamp %=
				repeat(4)[digit] >> char_("/")
				>> repeat(3)[digit]
				>> repeat(3)[ char_(":") >> qi::repeat(2)[digit] ]
				;

			one_liner %= lexeme[+(char_ - qi::eol)];

			event_name %= string("Survey") | string("Obs") | string("Profile") ;

			event_type %= string("Begin") | string("End") ;

			obsid %= lexeme[ string("Global") | +(digit | char_("-")) ];

			obsnum %= lit("obs_number") >> "=" >> obsid;

			prop_ID %= lit("//") >> lit("prop_ID") >> "=" >> +digit ;

			target_name %= lit("//") >> lit("target_name") >> "=" >> one_liner ;

			PI %= lit("//") >> lit("PI") >> "=" >> one_liner ;

			comment %= lit("//") >> lit("comment") >> "=" >> one_liner ;

			offset %= lit("//") >> lit("offset") >> "=" >> qi::double_ >> lit("deg") ;

			RA %= lit("//") >> lit("RA") >> "=" >> qi::double_ >> -lit("deg") ;

			DEC %= lit("//") >> qi::omit[ qi::no_case["DEC"] ] >> "="
				>> qi::double_ >> -lit("deg") ;

			week %= lit("//") >> lit("week") >> "=" >> qi::int_ ;

			SSN %= lit("//") >> lit("SSN") >> "=" >> qi::int_ ;

			duration %= lit("//") >> lit("duration") >> "=" >> qi::double_
				>> lit("ksec") ;

			slew %= lit("//") >> lit("slew") >> "=" >> qi::int_ >> lit("sec") ;

			saa %= lit("//") >> lit("saa") >> "=" >> qi::int_ >> lit("sec") ;

			rocktime_angle %= lit("//") >> qi::omit[digit >> digit] >> int_ >> double_ ;

			rocking_profile %=
				lit("//") >> lit("Rocking Profile:")
				>> lit("//")>>lit("ROCKSTART") >> "=" >> timestamp >> '(' >> int_ >> ')'
				>> lit("//") >> lit("ROCKDEFAULT") >> "=" >> double_
				>> lit("//") >> lit("ROCKTIME") >> lit("ROCKANGLE")
				>> repeat(17)[rocktime_angle]
				;

			opt_evt_fields %=
				prop_ID ^ target_name ^ PI ^ comment ^ offset ^ RA ^ DEC ^ RA ^ DEC
				^ week ^ SSN ^ duration ^ slew ^ saa ^ rocking_profile ;

			event %=
				lit("//")
				>> timestamp >> event_name >> event_type >> obsnum >> -opt_evt_fields
				;

			file_path %=
				lexeme[ +( -char_("/") >> +(qi::graph - char_("/") - char_(".")) )
				>> char_(".") >> +qi::alnum ];
			/* file_path %= lexeme[ +(qi::graph - char_("/") - qi::eol) ]; */
			/* file_path %= string("TOKEN"); */

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

			timeline %=
				-initial
				>> +event
				>> qi::eoi
				;
		}

		// Start Rule for timelines
		qi::rule<Iterator, timeline(), ascii::space_type> timeline;

		// Components of a timeline
		qi::rule<Iterator, timeline_event(), ascii::space_type> event;

		// Optional fields of event struct
		qi::rule<Iterator, opt_evt_fields(), ascii::space_type> opt_evt_fields;

		// The rocking profile
		qi::rule<Iterator, rocking_profile(), ascii::space_type> rocking_profile;

		// The time,angle pair of a rocking profile
		qi::rule<Iterator, rockprofile_pair(), ascii::space_type> rocktime_angle;

		// The initial annotation section containing initial pointing and
		// initial survey rocking profile
		qi::rule<Iterator, initial(), ascii::space_type> initial;

		// Sub-parsers
		typedef qi::rule<Iterator, std::string(), ascii::space_type> string_rule;
		typedef qi::rule<Iterator, int, ascii::space_type> int_rule;
		typedef qi::rule<Iterator, double, ascii::space_type> double_rule;

		string_rule one_liner;
		string_rule file_path;

		string_rule timestamp;
		string_rule event_name;
		string_rule event_type;
		string_rule obsnum;
		string_rule obsid;

		string_rule prop_ID;
		string_rule target_name;
		string_rule PI;
		string_rule comment;

		double_rule RA;
		double_rule DEC;
		double_rule offset;
		double_rule duration;

		int_rule week;
		int_rule SSN;
		int_rule slew;
		int_rule saa;

	};
	//]
}

int main(int argc, char **argv)
{
	char const* filename;
	if (argc > 1)
	{
		filename = argv[1];
	}
	else
	{
		std::cerr << "Error: No input file provided." << std::endl;
		return 1;
	}

	std::ifstream in(filename, std::ios_base::in);

	if (!in)
	{
		std::cerr << "Error: Could not open input file: "
			<< filename << std::endl;
		return 1;
	}

	std::string storage; // We will read the contents here.
	in.unsetf(std::ios::skipws); // No white space skipping!
	std::copy(
			std::istream_iterator<char>(in),
			std::istream_iterator<char>(),
			std::back_inserter(storage));

	typedef client::timeline_grammar<std::string::const_iterator> tlg;
	tlg g;
	client::timeline tl; // Our tree

	using boost::spirit::ascii::space;
	std::string::const_iterator iter = storage.begin();
	std::string::const_iterator end = storage.end();
	bool r = phrase_parse(iter, end, g, space, tl);

	if (r && iter == end)
	{
		std::cout << "-------------------------\n";
		std::cout << "Parsing succeeded\n";
		std::cout << "-------------------------\n";
		for (auto evt : tl.events ){
			/* std::cout << "got: " << boost::fusion::as_vector(evt) << std::endl; */
			std::cout
				<< evt.timestamp << " "
				<< evt.event_name << " "
				<< evt.event_type << " "
				<< evt.obs_number << " \n"
				<< "\tpropid     : " << evt.additional.prop_ID << std::endl
				<< "\ttarget_name: " << evt.additional.target_name << std::endl
				<< "\tPI         : " << evt.additional.PI << std::endl
				<< "\tcomment    : " << evt.additional.comment << std::endl
				<< "\toffset     : " << evt.additional.offset  << std::endl
				<< "\tRA         : " << evt.additional.RA      << std::endl
				<< "\tDEC        : " << evt.additional.DEC     << std::endl
				<< "\tweek       : " << evt.additional.week    << std::endl
				<< "\tSSN        : " << evt.additional.SSN    << std::endl
				<< "\tduration   : " << evt.additional.duration    << std::endl
				<< "\tslew       : " << evt.additional.slew    << std::endl
				<< "\tsaa        : " << evt.additional.saa    << std::endl
				<< "\t\trockstart		: " << evt.additional.profile.rockstart    << std::endl
				<< "\t\trockstart_met		: " << evt.additional.profile.rockstart_met    << std::endl
				<< "\t\trockdefault		: " << evt.additional.profile.rockdefault    << std::endl
				;
				for (auto rockpair : evt.additional.profile.pairs) {
					std::cout
					<< "\t\trockpair  : "<<rockpair.rocktime<<" "
					<< rockpair.rockangle << std::endl
					;
				}
				std::cout << std::endl;
		}
		std::cout << tl.init.week << std::endl;
		std::cout << tl.init.timeline_name << std::endl;
		std::cout << tl.init.create_time << std::endl;
		std::cout << tl.init.creator << std::endl;
		std::cout << tl.init.RA << std::endl;
		std::cout << tl.init.DEC << std::endl;
		for (auto db : tl.init.tako_db) std::cout <<"\t"<< db << std::endl;
		std::cout << std::endl;
		for (auto db : tl.init.sc_ephem)std::cout <<"\t"<< db << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		for (auto db : tl.init.saa) std::cout <<"\t"<< db << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		for (auto db : tl.init.tdrss_ephem) std::cout <<"\t"<< db << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		for (auto db : tl.init.tdrss_sched) std::cout <<"\t"<< db << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << tl.init.prev_arr_thresh << std::endl;
	}
	else
	{
		std::string::const_iterator some = iter+256;
		std::string context(iter, (some>end)?end:some);
		std::cout << "-------------------------\n";
		std::cout << "Parsing failed\n";
		std::cout << "stopped at: \": " << context << "...\"\n";
		std::cout << "-------------------------\n";
		return 1;
	}
}
