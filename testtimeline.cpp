
#include <iostream>
#include <vector>

#include "timeline.h"


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

	timeline tl = timeline(filename);

	bool r = tl.success;

	if (r)  // && iter == end)
	{
		std::cout << "-------------------------\n";
		std::cout << "Parsing succeeded\n";
		std::cout << "-------------------------\n";
		std::cout << tl.header.filename << std::endl;
		std::cout << tl.header.creation_time << std::endl;
		std::cout << tl.header.mission_id << std::endl;
		std::cout << tl.header.originator << std::endl;
		std::cout << tl.header.db_version << std::endl;
		std::cout << tl.header.dest_processor << std::endl;
		std::cout << tl.header.start_time << std::endl;
		std::cout << tl.header.stop_time << std::endl;
		std::cout << tl.header.execute_flag << std::endl;
		std::cout << tl.header.timeline_type << std::endl;
		std::cout << tl.header.version_num << std::endl;
		std::cout << tl.header.ref_timeline_name << std::endl;
		std::cout << tl.header.comment << std::endl;
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
				/* std::cout << std::endl; */
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
		/* std::string::const_iterator some = iter+256; */
		/* std::string context(iter, (some>end)?end:some); */
		std::cout << "-------------------------\n";
		std::cout << "Parsing failed\n";
		/* std::cout << "stopped at: \": " << context << "...\"\n"; */
		std::cout << "-------------------------\n";
		return 1;
	}
}
