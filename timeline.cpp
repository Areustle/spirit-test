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

	struct rocking_profile
	{
		std::string rockstart;
		int rockstart_met;
		double rockdefault;
		std::vector<int> rocktime;
		std::vector<double> rockangle;
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
		rocking_profile rock;
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

	struct timeline
	{
		std::vector<timeline_event> events;
	};
}

// We need to tell fusion about our timeline struct
// to make it a first-class fusion citizen. This has to
// be in global scope.
BOOST_FUSION_ADAPT_STRUCT(
	client::rocking_profile,
	(std::string, rockstart)
	(int, rockstart_met)
	(double, rockdefault)
	/* (std::vector<int>, rocktime) */
	/* (std::vector<double>, rockangle) */
)

BOOST_FUSION_ADAPT_STRUCT(
	client::opt_evt_fields,
	(std::string, prop_ID)
	(std::string, target_name)
	(std::string, PI)
	(std::string, comment)
	(double, offset)
	(double, RA)
	(double, DEC)
	(int, week)
	(int, SSN)
	(double, duration)
	(int, slew)
	(int, saa)
	(client::rocking_profile, rock)
)

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

			event_name %=
				string("Survey")
				| string("Obs")
				| string("Profile")
				;

			event_type %=
				string("Begin")
				| string("End")
				;

			obsid %= lexeme[ string("Global") | +(digit | char_("-")) ];

			obsnum %= lit("obs_number") >> "=" >> obsid;

			prop_ID %=
				lit("//")
				>> lit("prop_ID")
				>> "="
				>> +digit
				;

			target_name %=
				lit("//")
				>> lit("target_name")
				>> "="
				>> lexeme[+(char_ - qi::eol) ]
				;

			PI %=
				lit("//")
				>> lit("PI")
				>> "="
				>> lexeme[+(char_ - qi::eol) ]
				;

			comment %=
				lit("//")
				>> lit("comment")
				>> "="
				>> lexeme[+(char_ - qi::eol) ]
				;

			offset %=
				lit("//")
				>> lit("offset")
				>> "="
				>> qi::double_
				>> lit("deg")
				;

			RA %=
				lit("//")
				>> lit("RA")
				>> "="
				>> qi::double_
				>> lit("deg")
				;

			DEC %=
				lit("//")
				>> lit("DEC")
				>> "="
				>> qi::double_
				>> lit("deg")
				;

			week %=
				lit("//")
				>> lit("week")
				>> "="
				>> qi::int_
				;

			SSN %=
				lit("//")
				>> lit("SSN")
				>> "="
				>> qi::int_
				;

			duration %=
				lit("//")
				>> lit("duration")
				>> "="
				>> qi::double_
				>> lit("ksec")
				;

			slew %=
				lit("//")
				>> lit("slew")
				>> "="
				>> qi::int_
				>> lit("sec")
				;

			saa %=
				lit("//")
				>> lit("saa")
				>> "="
				>> qi::int_
				>> lit("sec")
				;


			rocking_profile %=
				lit("//") >> lit("Rocking Profile:")
				>> lit("//") >> lit("ROCKSTART") >> "=" >> timestamp >> '(' >> int_ >> ')'
				>> lit("//") >> lit("ROCKDEFAULT") >> "=" >> double_
				/* >> lit("//") >> lit("ROCKTIME") >> lit("ROCKANGLE") */
				;

			opt_evt_fields %=
				prop_ID
				^ target_name
				^ PI
				^ comment
				^ offset
				^ RA
				^ DEC
				^ week
				^ SSN
				^ duration
				^ slew
				^ saa
				^ rocking_profile
				;

			event %=
				lit("//")
				>> timestamp
				>> event_name
				>> event_type
				>> obsnum
				>> -opt_evt_fields
				;

			timeline %= +event >> qi::eoi;
		}

		// Start Rule for timelines
		qi::rule<Iterator, timeline(), ascii::space_type> timeline;

		// Components of a timeline
		qi::rule<Iterator, timeline_event(), ascii::space_type> event;

		// Sub-parsers
		typedef qi::rule<Iterator, std::string(), ascii::space_type> string_rule;
		typedef qi::rule<Iterator, int, ascii::space_type> int_rule;
		typedef qi::rule<Iterator, double, ascii::space_type> double_rule;

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

		qi::rule<Iterator, rocking_profile(), ascii::space_type> rocking_profile;
		qi::rule<Iterator, opt_evt_fields(), ascii::space_type> opt_evt_fields;
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
				<< "\t\trockstart		: " << evt.additional.rock.rockstart    << std::endl
				<< "\t\trockstart_met		: " << evt.additional.rock.rockstart_met    << std::endl
				<< "\t\trockdefault		: " << evt.additional.rock.rockdefault    << std::endl
				<< std::endl;
		}
		return 0;
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
