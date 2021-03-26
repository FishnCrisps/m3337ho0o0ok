#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"
#include "clipboard_helpers.hpp"
#include <vector>
#include "errorhandling_hooks.hpp"
#include "gameapi.hpp"
void idlib_dump(idCmdArgs* args) {
	idType::do_idlib_dump();
	return;
}

void event_dump(idCmdArgs* args) {
	DumpEventDefs();
}
void* __fastcall idFileResourceCompressed__GetFile(__int64 a1);


extern void cmd_mh_spawninfo(idCmdArgs* args);
extern void cmd_mh_ang2mat(idCmdArgs* args);

void cmd_noclip(idCmdArgs* args) {
	auto player = find_entity("player1");

	if(!player) {
		idLib::Printf("You don't exist. Wait until you spawn.\n");
		return;
	}
	reinterpret_cast<void (*)(uintptr_t, void*)>(descan::g_noclip_func)(0, player);
}


void cmd_notarget(idCmdArgs* args){
	auto player = find_entity("player1");
	if(!player) {
		idLib::Printf("No player1 instance available.\n");
		return;
	}


	auto headattr = idType::FindClassField("idPlayer", "noTarget");
	if(!headattr) {
		idLib::Printf("Couldn't locate the noTarget property on idPlayer, tell chrispy.\n");
		return;
	}

	int notarget_value = headattr->get(player);

	notarget_value = !notarget_value;
	if(notarget_value) {
		idLib::Printf("Turning notarget on.\n");
	}
	else 
		idLib::Printf("Disabling notarget\n");

	headattr->set(player, notarget_value);

}
/*
struct idEntityHierarchy {
	//Offset 8,	 size 48
	//Parent
	idStr parent;
	//Offset 56,	 size 24
	//Children
	idList < idEntityHierarchy::childInfo_t , TAG_IDLIST , false > childrenInfo;
};
struct idMapEntityEditorData {
*/
static void* find_entity_in_mapfile(const char* entname) {
	void* levelmap = call_as<void*>(descan::g_maplocal_getlevelmap, *reinterpret_cast<void**>(descan::g_gamelocal));

	auto entities_field = idType::FindClassField("idMapFileLocal", "entities");

	char* entities_ptr = ((char*)levelmap) + entities_field->offset;

	auto entities_list_offset = idType::FindClassField(entities_field->type, "list");

	auto entities_num_offset = idType::FindClassField(entities_field->type, "num");


	void** entities_array = *(void***)(entities_ptr + entities_list_offset->offset);

	int numentities = *(int*)(entities_ptr + entities_num_offset->offset);


	auto editordata_field = idType::FindClassField("idMapEntityLocal", "mapEntityEditorData");

	auto original_name_field = idType::FindClassField("idMapEntityLocal", "model");


	auto mapentityeditordata_hierarchy = idType::FindClassField("idMapEntityEditorData", "entityHiearchy");

	auto entityhierarchy_parent = idType::FindClassField("idEntityHierarchy", "parent");

	
	for(int i = 0; i < numentities; ++i) {
		
		void* current_ent = entities_array[i];
		/*
		char* editordata = *(char**)(((char*)current_ent) + editordata_field->offset);
		if(!editordata)
			continue;

		idStr* parentptr = (idStr*)((mapentityeditordata_hierarchy->offset + entityhierarchy_parent->offset) + editordata);
		if(parentptr->data) {
			idLib::Printf(parentptr->data);

			if(!strcmp(parentptr->data, entname)) {
				return current_ent;
			}
		}*/
		idStr* originalName = (idStr*) (((char*)current_ent) + original_name_field->offset);

		if(originalName->data) {
			idLib::Printf(originalName->data);

			if(!strcmp(originalName->data, entname)) {
				return current_ent;
			}
		}
	}

	return nullptr;

}
#include <string>
void cmd_mh_query_type(idCmdArgs* args) {
	if(args->argc<2)
		return;
	auto t = idType::FindClassInfo(args->argv[1]);
	std::string clipdata{};
	char scratchbuf[2048];

	if(!t){
	
		auto e = idType::FindEnumInfo(args->argv[1]);

		if(!e) {
			idLib::Printf("Type %s was not found.\n", args->argv[1]);
			return;
		}

		for(auto eval = e->values; eval && eval->name && eval->name[0]; eval++) {
			sprintf_s(scratchbuf, "\t%s\n", eval->name);

			clipdata+=scratchbuf;

			idLib::Printf(scratchbuf);
		}
		idLib::Printf("Dumped type is a Enum\n");
	}
	else {

		for (auto bleh2 = t->variables; bleh2 && bleh2->name; ++bleh2) {

			sprintf_s(scratchbuf, "\t%s%s %s\n",bleh2->type, bleh2->ops, bleh2->name);

			if(bleh2->comment && bleh2->comment[0]) {
				clipdata+="\n\t//";
				clipdata+=bleh2->comment;
				clipdata+="\n";
			}
			clipdata += scratchbuf;
			idLib::Printf(scratchbuf);
		}

		idLib::Printf("\nDumped type is a Class\n");
		if(t->superType && t->superType[0]) {
			idLib::Printf("\nFor more fields this class has, use mh_type on its superclass, %s\n", t->superType);
		}
	}
	set_clipboard_data(clipdata.c_str());
}

void cmd_mh_printentitydef(idCmdArgs* args) {

	if(args->argc < 2) {
		idLib::Printf("Not enough args.\n");
		return;
	}
	idLib::Printf(args->argv[1]);

	void* ent = find_entity(args->argv[1]);

	if(!ent) {
		idLib::Printf("Couldn't find entity %s.\n", args->argv[1]);
		return;
	}

	auto entdef_field = idType::FindClassField("idEntity", "entityDef");
	if(!entdef_field) {
		idLib::Printf("Couldn't locate entityDef field!! This is bad.\n");
		return;
	}

	void* edef = *reinterpret_cast<void**>(reinterpret_cast<char*>(ent) + entdef_field->offset);

	if(!edef){
		idLib::Printf("Entity %s's entitydef is null.\n", args->argv[1]);
		return;

	}

	idStr outstr;
	call_as<void>(descan::g_declentitydef_gettextwithinheritance, edef, &outstr, false);

	idLib::Printf("%s\n", outstr.data);
	set_clipboard_data(outstr.data);


}


void cmd_mh_dumpmapfile(idCmdArgs* args) {
	if(args->argc < 2) {
		idLib::Printf("Not enough args.\n");
		return;	
	}
	void* levelmap = call_as<void*>(descan::g_maplocal_getlevelmap, *reinterpret_cast<void**>(descan::g_gamelocal));
	if(!levelmap) {
		idLib::Printf("GetLevelMap returned null.\n");
		return;
	}

	call_as<void>(descan::g_idmapfile_write, levelmap, args->argv[1]);

}

void cmd_mh_testmapentity(idCmdArgs* args) {
	
	void* ptr = find_entity_in_mapfile(args->argv[1]);

	idLib::Printf("%p\n", ptr);

}

void cmd_mh_forcereload(idCmdArgs* args)
{
	// Force a reload of the file. idGameSystemLocal->persistMapFile
	memset(((char*)descan::g_idgamesystemlocal + 0x60), 0, sizeof(void*));

	// idGameSystemLocal->idGameSystemLocal::mapChangeRequest_t->requested
	bool* mapChangeRequest_t_requested = ((bool*)((char*)descan::g_idgamesystemlocal + 0xB8));
	*mapChangeRequest_t_requested = true;
}

std::vector<std::string> gActiveEncounterNames;
void cmd_active_encounter(idCmdArgs* args)
{
	// Following chain: idGameSystemLocal->idSaveGameSerializer->idSerializeFunctor<idMetrics>->idMetrics->idPlayerMetrics[0]
	long long* idSaveGameSerializer = ((long long*)((char*)descan::g_idgamesystemlocal + 0xB0));
	long long* idSerializerFunctor = (long long*)(*(idSaveGameSerializer));
	long long* idMetrics = (long long*)(*(idSerializerFunctor + 0x59)); // 0x2C8
	long long* idPlayerMetrics = (long long*)(*(idMetrics + 1)); // 0x8
	long long* EncounterArray = (long long*)(*(idPlayerMetrics + 5)); // 0x28
	long long* Encounter = (long long*)(*(EncounterArray));

	long long EncounterListSize = *(Encounter - 1) - (4 * 0x8); // This seems to work but looks a bit fragile.
	long long* EncounterManager = Encounter + 1; // Skip first encounter which always points to an instance of idGameChallenge_CampaignSinglePlayer.
	char* Name = nullptr;
	size_t Index = 1;
	while ((long long)Index < (EncounterListSize / 8)) {
		// Active
		if (EncounterManager == 0) {
			break;
		}

		long long *Encounter = ((long long*)(*EncounterManager));
		if (Encounter == nullptr) {
			break;
		}

		int Active = *((int*)(Encounter + 0xC5)); // 0x628 -- Fragile offset for determining if an encounter is active.
		if (Active != 0) {
			char* Name = (char*)*(Encounter + 0x9); // 0x48 -- Encounter manager pointer to name string.
			if (strcmp("player1", Name) == 0) {
				break;
			}

			char String[MAX_PATH];
			sprintf_s(String, "Active name %s\n", Name);
			OutputDebugStringA(String);
			idLib::Printf(String);

			// Name
			gActiveEncounterNames.push_back(Name);
		}
		EncounterManager++;
		Index += 1;
	}
}

std::string gCurrentCheckpointName;
void cmd_current_checkpoint(idCmdArgs* args)
{
	// Following chain: idGameSystemLocal->idGameSpawnInfo->checkpointName
	long long* idMapInstanceLocal = ((long long*)((char*)descan::g_idgamesystemlocal + 0x50));
	
	char String[MAX_PATH*10];
	// (char*)((*((longlong*)((char*)descan::g_idgamesystemlocal + 0x50))) + 0x1108)

	char* Name = (char*)"NULL";
	if (idMapInstanceLocal != nullptr) {
		Name = (char*)((*(idMapInstanceLocal)) + 0x1108); // checkpoint string. 0x1108 byte
	}

	if (Name == nullptr || Name[0] == 0) {
		Name = (char*)"<no checkpoint loaded>";
	}

	sprintf_s(String, "Current checkpoint name %s\n", Name);
	OutputDebugStringA(String);
	idLib::Printf(String);
	gCurrentCheckpointName = Name;
}

void cmd_optimize(idCmdArgs* args) {
	make_ret(descan::g_security_check_cookie);
	make_ret(descan::g_alloca_probe);

	/*
		sqrtss xmm0, xmm0
		ret
	*/
	char sqrtf_override[] = { 0xF3, 0x0F, 0x51, 0xC0, 0xC3 };

	patch_memory_with_undo(descan::g_sqrtf, sizeof(sqrtf_override), sqrtf_override);

	char sqrt_override[] = { 0xF2, 0x0F, 0x51, 0xC0, 0xC3 };

	patch_memory_with_undo(descan::g_sqrt, sizeof(sqrtf_override), sqrt_override);
}

/*
	can use mapentity/maplocal typeinfo with getlevelmap to facilitate easier editing

*/

void meathook_init() {
	redirect_to_func((void*)idFileResourceCompressed__GetFile, (uintptr_t)/* doomsym<void>(doomoffs::idFileResourceCompressed__GetFile)*/ descan::g_idfilecompressed_getfile, true);
	install_error_handling_hooks();

	unsigned patchval_enable_commands = 0;

	patch_memory(descan::g_command_patch_area, 4, (char*)&patchval_enable_commands);
	idCmd::register_command("mh_spawninfo", cmd_mh_spawninfo, "Copy your current position and orientation, formatted as spawnPosition and spawnOrientation to the clipboard");
	idCmd::register_command("noclip", cmd_noclip, "Toggle noclip");
	idCmd::register_command("notarget", cmd_notarget, "Toggle notarget");
	idCmd::register_command("mh_printentitydef", cmd_mh_printentitydef, "Print the entitydef of the entity with the provided name to the console");
	idCmd::register_command("mh_dumpmap", cmd_mh_dumpmapfile, "Dump current idMapLocal to a .map file.");
	idCmd::register_command("mh_type", cmd_mh_query_type, "Dump fields for provided class");
	idCmd::register_command("mh_force_reload", cmd_mh_forcereload, "Force reload current level");
	idCmd::register_command("mh_active_encounter", cmd_active_encounter, "Get the list of active encounter managers");
	idCmd::register_command("mh_current_checkpoint", cmd_current_checkpoint, "Get the current checkpoint name");
	idCmd::register_command("mh_ang2mat", cmd_mh_ang2mat, "mh_ang2mat pitch yaw roll : converts the pitch, yawand roll values for idAngles to a decl - formatted matrix, copying the result to your clipboard");
	idCmd::register_command("mh_optimize", cmd_optimize, "Patches the engine to make stuff run faster. do not use online");

	idCmd::register_command("idlib_dump",idlib_dump, "idlib_dump");
}