#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"
#include <cassert>
void* find_entity(const char* name) {
	void* gamloc = *reinterpret_cast<void**>(descan::g_gamelocal);

	void* vftbl = reinterpret_cast<void**>(gamloc)[0];

	void* findentity_func = (((char*)vftbl) + 0x78);
	findentity_func = *(void**)findentity_func;

	return reinterpret_cast<void* (*)(void*, const char*)>(findentity_func)(gamloc, name);
}
void* find_next_entity_with_class(const char* classname, void* entity_iterator) {
	void* gamloc = *reinterpret_cast<void**>(descan::g_gamelocal);

	return call_as<void*>(descan::g_find_next_entity_with_class, gamloc, entity_iterator, classname);
}

void* get_level_map() {
#if !defined(MH_ETERNAL_V6)
	return call_as<void*>(descan::g_maplocal_getlevelmap, *reinterpret_cast<void**>(descan::g_gamelocal));
#else
	void* gamelocal = *reinterpret_cast<void**>(descan::g_gamelocal);

	char* gamelocal_vftbl = *reinterpret_cast<char**>(gamelocal);

	void* getlevelmap_func = *reinterpret_cast<void**>(gamelocal_vftbl + descan::g_vftbl_offset_getlevelmap);

	return call_as<void*>(getlevelmap_func, gamelocal);
#endif
}
bool get_classfield_boolean(void* obj, const classVariableInfo_t* varinfo) {
	if (varinfo->get) {
		return varinfo->get(obj);
	}
	else {
		return *reinterpret_cast<bool*>(reinterpret_cast<char*>(obj) + varinfo->offset);
	}
}

void set_classfield_boolean(void* obj, const classVariableInfo_t* varinfo, bool value) {
	if (varinfo->set) {
		varinfo->set(obj, value);
	}
	else {
		*reinterpret_cast<bool*>(reinterpret_cast<char*>(obj) + varinfo->offset) = value;
	}
}
long long get_classfield_int(void* obj, const classVariableInfo_t* varinfo) {
	if (varinfo->get) {
		return varinfo->get(obj);
	}
	else {
		char* objptr =reinterpret_cast<char*>(obj) + varinfo->offset;


		switch(varinfo->size) {
		case 1:
			return *objptr;
		case 2:
			return *reinterpret_cast<short*>(objptr);

		case 4:
			return *reinterpret_cast<int*>(objptr);
		case 8:
			return *reinterpret_cast<long long*>(objptr);
		}
		assert(false);
		return 0;
	}
}
long long get_classfield_int(void* obj, const char* clazs, const char* field) {
	auto inf = idType::FindClassField(clazs, field);
	return get_classfield_int(obj, inf);
}
bool get_classfield_boolean(void* obj, const char* clazs, const char* field) {
	auto inf = idType::FindClassField(clazs, field);
	return get_classfield_boolean(obj, inf);
}
const char* get_entity_name(void* obj) {
	return reinterpret_cast<idStr*>(reinterpret_cast<char*>(obj) + idType::FindClassField("idEntity", "name")->offset)->data;
}