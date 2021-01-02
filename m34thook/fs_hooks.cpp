#include <Windows.h>
#include "game_exe_interface.hpp"
#include "memscan.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "scanner_core.hpp"
#include "fs_hooks.hpp"
#include "memorySystem.hpp"
#include <map>
#include <string>
#include <vector>
//idFileSystemLocal::ReadFile(char const*,void **,idFileProps *,fsPathSearch_t)	.text	0000007101D87CB0	00000178	00000040	FFFFFFFFFFFFFFF8	R	.	.	.	B	T	.

static FILE* g_readfile_log = nullptr;
extern const char* gOverrideFileName;
std::string gLastLoadedEntities;
extern std::string gOverrideName;
FILE* get_override_for_resource(const char* name, size_t* size_out) {
	FILE* resfile = nullptr;
	char pathbuf[4096];
	if ((gOverrideFileName != nullptr) && (strstr(name, ".entities") != 0) && (gOverrideName == std::string(name))) {
		strcpy_s(pathbuf, gOverrideFileName);
		gOverrideFileName = nullptr;

	} else {
		sprintf_s(pathbuf, ".\\overrides\\%s", name);
	}

	if (strstr(name, ".entities") != 0) {
		gLastLoadedEntities = name;
	}

	unsigned i = 0;
	for (unsigned i = 0; pathbuf[i]; ++i)
		if (pathbuf[i] == '/')
			pathbuf[i] = '\\';

	fopen_s(&resfile, pathbuf, "rb");
	if (!resfile)
		return nullptr;

	fseek(resfile, 0, SEEK_END);

	*size_out = ftell(resfile);

	fseek(resfile, 0, SEEK_SET);
	return resfile;
}
static constexpr unsigned INDEX_OF_READFILE = 29;
static size_t readfile_repl(void* fs, const char* path, void** out_data, void* fileprops, unsigned psrch);
static void* g_old_readfile = (void*)readfile_repl;

static void dispose_log() {
	fclose(g_readfile_log);
}
static size_t readfile_repl(void* fs, const char* path, void** out_data, void* fileprops, unsigned psrch) {
	
/*	if(!g_readfile_log) {
		fopen_s(&g_readfile_log, "Creadfile_eternal.txt", "w");
		atexit(dispose_log);
	}*/

	size_t res = call_as<size_t>(g_old_readfile, fs, path, out_data, fileprops, psrch);

	size_t sz = 0;
	FILE* overrid = get_override_for_resource(path, &sz);

//	fprintf(g_readfile_log, "%s\n", path);
	if(!overrid)
		return res;

//	idMemorySystem_free(*out_data);


	void* newdata = idMemorySystem_malloc(sz, 52, 0);

	memset(newdata, 0, sz);
	fread(newdata, 1, sz, overrid);
	fclose(overrid);
	*out_data = newdata;

	return res;

}



void hook_idfilesystem() {
	void** fs_vftbl = **reinterpret_cast<void****>(descan::g_idfilesystemlocal);

	swap_out_ptrs(&fs_vftbl[INDEX_OF_READFILE], &g_old_readfile, 1);


}