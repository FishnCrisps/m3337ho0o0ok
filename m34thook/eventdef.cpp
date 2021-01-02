
#include "eventdef.hpp"
#include <string>
#include <vector>
idEventDefInterfaceLocal* idEventDefInterfaceLocal::Singleton() {
	return nullptr;//doomsym<idEventDefInterfaceLocal>(doomoffs::eventdef_interface);
}

static std::string event_to_string(idEventDefInterfaceLocal* iface, unsigned evnum) {
	
	const char* name = iface->GetEventNameForNum(evnum);

	struct arginfo_t {
		std::string name;
		std::string type;
	};

	std::vector<arginfo_t> allargs{};

	unsigned narg = iface->GetNumEventArgs(evnum);
	allargs.reserve(narg);
	

	for(unsigned i = 0; i < narg; ++i) {
		arginfo_t info{};
		{
			idStr temp{};
			iface->GetEventArgName(evnum, i, &temp);
			info.name = temp.data;
		}
		{
			idStr temp{};
			iface->GetEventArgTypeName(evnum, i, &temp);
			info.type = temp.data;
		}

		allargs.push_back(std::move(info));
	}


	std::string result = name;
	result+= ":\n";

	for(auto&& arg:  allargs) {
		result+="\t";
		result+=arg.name;
		result+=" : ";
		result+=arg.type;
		result+="\n";

	}

	return result;
}

void DumpEventDefs() {
	auto iface = idEventDefInterfaceLocal::Singleton();

	unsigned nevent = iface->GetNumEvents();

	std::string result_text{};

	for(unsigned i = 0; i <nevent; ++i) {
		result_text += event_to_string(iface, i);
		result_text+="\n";
	}
	FILE* outfile = nullptr;
	fopen_s(&outfile, "eternalevents.txt", "w");
	fputs(result_text.c_str(), outfile);

	fclose(outfile);



}