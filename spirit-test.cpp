/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A mini XML-like parser
//
//  [ JDG March 25, 2007 ]   spirit2
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <complex>

namespace client
{
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

		/* struct rocking_profile */
		/* { */
		/* 	std::string rockstart; */
		/* 	std::string rockdefault; */
		/* 	std::vector<int> rocktime; */
		/* 	std::vector<double> rockangle; */
		/* }; */

		/* struct opt_evt_fields */
		/* { */
		/* 	std::string prop_ID; */
		/* 	std::string target_name; */
		/* 	std::string offset; */
		/* 	int target_RA; */
		/* 	int target_DEC; */
		/* 	std::string PI; */
		/* 	std::string comment; */
		/* 	int week; */
		/* 	std::string SSN; */
		/* 	int duration; */
		/* 	int slew; */
		/* 	int saa; */
		/* 	int dup_RA; */
		/* 	int dup_DEC; */
		/* 	rocking_profile rock; */
		/* }; */

		struct timeline_event
		{
			//Required fields
			std::string timestamp;
			std::string event_name;
			std::string event_type;
			std::string obs_number;

			//Optional fields
			/* opt_evt_fields additional; */
		};

		/* struct timeline */
		/* { */
		/* 	std::vector<timeline_event> events; */
		/* }; */
}

BOOST_FUSION_ADAPT_STRUCT(
		client::timeline_event,
		(std::string, timestamp)
		(std::string, event_name)
		(std::string, event_type)
		(std::string, obs_number)
)

namespace client
{
		template <typename Iterator>
		struct timeline_grammar : qi::grammar<Iterator, timeline_event(), ascii::space_type>
		{
			timeline_grammar() : timeline_grammar::base_type(event)
			{
				using qi::lit;
				using qi::lexeme;
				using ascii::char_;

				event_name %= lexeme['"' >> +(char_ - '"') >> '"'];
					/* qi::string("Surevey") */
					/* | qi::string("Obs") */
					/* | qi::string("Profile") */
					/* ; */

				event_type %= lexeme['"' >> +(char_ - '"') >> '"'];
					/* qi::string("Begin") */
					/* | qi::string("End") */
					/* ; */

				event %=
					lit("//")
					>> lexeme[ +ascii::char_ ]
					>> event_name
					>> event_type
					>> lit("obs_number") >> lexeme[ +char_ ]
					;
			}

      qi::rule<Iterator, std::string(), ascii::space_type> event_name;
      qi::rule<Iterator, std::string(), ascii::space_type> event_type;
      qi::rule<Iterator, timeline_event(),  ascii::space_type> event;
		};
}

/////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////
int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tAn employee parser for Spirit...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";

    std::cout
        << "Give me an employee of the form :"
        << "employee{age, \"surname\", \"forename\", salary } \n";
    std::cout << "Type [q or Q] to quit\n\n";

    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef client::timeline_grammar<iterator_type> timeline_grammar;

    timeline_grammar g; // Our grammar
    /* std::string str; */
    /* while (getline(std::cin, str)) */
    /* { */
    /*     if (str.empty() || str[0] == 'q' || str[0] == 'Q') */
    /*         break; */

    /*     client::timeline_event emp; */
    /*     std::string::const_iterator iter = str.begin(); */
    /*     std::string::const_iterator end = str.end(); */
    /*     bool r = phrase_parse(iter, end, g, space, emp); */

    /*     if (r && iter == end) */
    /*     { */
    /*         std::cout << boost::fusion::tuple_open('['); */
    /*         std::cout << boost::fusion::tuple_close(']'); */
    /*         std::cout << boost::fusion::tuple_delimiter(", "); */

    /*         std::cout << "-------------------------\n"; */
    /*         std::cout << "Parsing succeeded\n"; */
    /*         std::cout << "got: " << boost::fusion::as_vector(emp) << std::endl; */
    /*         std::cout << "\n-------------------------\n"; */
    /*     } */
    /*     else */
    /*     { */
    /*         std::cout << "-------------------------\n"; */
    /*         std::cout << "Parsing failed\n"; */
    /*         std::cout << "-------------------------\n"; */
    /*     } */
    /* } */

    std::cout << "Bye... :-) \n\n";
    return 0;
}//////////////////////////////////////////////////////////////////////////////
