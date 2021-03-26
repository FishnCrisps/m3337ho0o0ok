#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"

void* find_entity(const char* name) {
	void* gamloc = *reinterpret_cast<void**>(descan::g_gamelocal);

	void* vftbl = reinterpret_cast<void**>(gamloc)[0];

	void* findentity_func = (((char*)vftbl) + 0x78);
	findentity_func = *(void**)findentity_func;

	return reinterpret_cast<void* (*)(void*, const char*)>(findentity_func)(gamloc, name);
}
