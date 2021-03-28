#include <Windows.h>
#include "game_exe_interface.hpp"
#include "memscan.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "scanner_core.hpp"
#include "fs_hooks.hpp"
#include "mh_native_api.hpp"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "idLib.hpp"
#include "clipboard_helpers.hpp"
#include "gameapi.hpp"
static mh_classtype_t find_classtype(const char* name) {
	return reinterpret_cast<mh_classtype_t>(idType::FindClassInfo(name));
}
static mh_enumtype_t find_enumtype(const char* name) {
	return reinterpret_cast<mh_enumtype_t>(idType::FindEnumInfo(name));
}


static bool enumval_iter_is_done(mh_enumval_t val) {
	enumValueInfo_t* eval = reinterpret_cast<enumValueInfo_t*>(val);
	return !(eval && eval->name && eval->name[0]);
}

static bool classvar_iter_is_done(mh_classvar_t var) {
	classVariableInfo_t* eval = reinterpret_cast<classVariableInfo_t*>(var);
	return !(eval && eval->name);
}

static const char* enumval_get_name(mh_enumval_t val){
	return reinterpret_cast<enumValueInfo_t*>(val)->name;
}

static long long enumval_get_value(mh_enumval_t val) {
	return reinterpret_cast<enumValueInfo_t*>(val)->value;	
}

static mh_enumval_t enumval_next(mh_enumval_t val) {
	return reinterpret_cast<mh_enumval_t>(reinterpret_cast<enumValueInfo_t*>(val) + 1);
}

static mh_classvar_t classvar_next(mh_classvar_t var) {
	return reinterpret_cast<mh_classvar_t>(reinterpret_cast<classVariableInfo_t*>(var) + 1);
}
/*
	char* type;
	char* ops;
	char* name;
	int offset;
	int size;
	int flags;
	char pad36[4];
	char* comment;
*/
static const char* classvar_type(mh_classvar_t var) {
	return reinterpret_cast<classVariableInfo_t*>(var)->type;
}
static const char* classvar_name(mh_classvar_t var) {
	return reinterpret_cast<classVariableInfo_t*>(var)->name;
}
static const char* classvar_ops(mh_classvar_t var) {
	return reinterpret_cast<classVariableInfo_t*>(var)->ops;
}
static const char* classvar_comment(mh_classvar_t var) {
	return reinterpret_cast<classVariableInfo_t*>(var)->comment;
}
static size_t classvar_size(mh_classvar_t var) {
	return reinterpret_cast<classVariableInfo_t*>(var)->size;
}
static size_t classvar_offset(mh_classvar_t var) {
	return reinterpret_cast<classVariableInfo_t*>(var)->offset;
}

static mh_game_entity_t find_entity_by_name(const char* entname) {
	return reinterpret_cast<mh_game_entity_t>(find_entity(entname));
}
static mh_interface_t g_interface{
	.m_execute_cmd_text = idCmd::execute_command_text,
	.m_console_printf = idLib::Printf,
	.m_get_clipboard_data = get_clipboard_data,
	.m_set_clipboard_data = set_clipboard_data,
	.m_mh_malloc = malloc,
	.m_mh_free = free,
	.m_mh_realloc = realloc,
	.m_find_classtype = find_classtype,
	.m_find_enumtype = find_enumtype,
	.m_enumval_iterator_is_done = enumval_iter_is_done,
	.m_classvar_iterator_is_done = classvar_iter_is_done,
	.m_enumval_get_name = enumval_get_name,
	.m_enumval_get_value = enumval_get_value,
	.m_enumval_next = enumval_next,
	.m_classvar_next = classvar_next,

	.m_classvar_type = classvar_type,
	.m_classvar_name = classvar_name,
	.m_classvar_ops = classvar_ops,
	.m_classvar_comment = classvar_comment,
	.m_classvar_size = classvar_size,
	.m_classvar_offset = classvar_offset,
	.m_find_entity_by_name = find_entity_by_name
};

/*
	we return a pointer to it instead of filling in a client side version of it. this incurs an extra indirection
	on the client codes side but ensures that future versions of the api will be backwards compatible with plugins written
	for an older version as long as functions are added to the end of the struct.
*/
extern "C"
__declspec(dllexport)
const mh_interface_t* mh_api_get() {
	return &g_interface;
}