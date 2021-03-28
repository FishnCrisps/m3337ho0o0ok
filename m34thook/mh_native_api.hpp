#pragma once

#define	MH_OPAQUE(name)	typedef struct{}*name

MH_OPAQUE(mh_classtype_t);

MH_OPAQUE(mh_classvar_t);

MH_OPAQUE(mh_enumtype_t);
MH_OPAQUE(mh_enumval_t);

/*
	an ingame entity at runtime
*/
MH_OPAQUE(mh_game_entity_t);

/*
	struct with pointers instead of virtual to ensure mingw compat
*/
struct mh_interface_t {
	/*
		warning: command text may not exceed 2048 bytes or the game will crash (see idCmdArgs)
	*/

	void (*m_execute_cmd_text)(const char* txt);

	void (*m_console_printf)(const char* format, ...);

	const char* (*m_get_clipboard_data)();
	void (*m_set_clipboard_data)(const char* data);

	//memory allocation functions that are assured to be compatible with this api
	void* (*m_mh_malloc)(size_t size);

	void (*m_mh_free)(void* vp);
	void* (*m_mh_realloc)(void* vp, size_t newsize);
	mh_classtype_t (*m_find_classtype)(const char* name);
	mh_enumtype_t (*m_find_enumtype)(const char* name);

	bool (*m_enumval_iterator_is_done)(mh_enumval_t val);
	bool (*m_classvar_iterator_is_done)(mh_classvar_t var);

	const char* (*m_enumval_get_name)(mh_enumval_t val);
	long long (*m_enumval_get_value)(mh_enumval_t val);

	mh_enumval_t (*m_enumval_next)(mh_enumval_t val);

	mh_classvar_t (*m_classvar_next)(mh_classvar_t var);

	const char* (*m_classvar_type)(mh_classvar_t var);
	const char* (*m_classvar_name)(mh_classvar_t var);
	const char* (*m_classvar_ops)(mh_classvar_t var);
	const char* (*m_classvar_comment)(mh_classvar_t var);

	size_t (*m_classvar_size)(mh_classvar_t var);
	size_t (*m_classvar_offset)(mh_classvar_t var);

	mh_game_entity_t (*m_find_entity_by_name)(const char* entity_name);
};