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
#include "idStr.hpp"
//idFileSystemLocal::ReadFile(char const*,void **,idFileProps *,fsPathSearch_t)	.text	0000007101D87CB0	00000178	00000040	FFFFFFFFFFFFFFF8	R	.	.	.	B	T	.
struct idFile;
enum fsLock_t {
	FS_LOCK_SHARED = 0,
	FS_LOCK_EXCLUSIVE = 1,
};
enum fsOrigin_t {
	FS_SEEK_CUR = 0,
	FS_SEEK_END = 1,
	FS_SEEK_SET = 2,
};
#if 1
struct idFileVftbl
{
	void (*dctor)(idFile* thiz, unsigned int);
	bool (*IsIDFile_Memory)(const idFile*);
	bool (*IsIDFile_Permanent)(const idFile*);
	//name is an assumption, only returns true for idfile_verified
	bool (*isIDFile_Verified)(const idFile*);
	const char* (*GetFullPath)(idFile* thiz);
	const char* (*GetName)(idFile* thiz);
	uint64_t(*Read)(idFile* thiz, void*, unsigned int len);
	uint64_t(*Write)(idFile* thiz, const void*, unsigned int);
	uint64_t(*ReadOfs)(idFile* thiz, long long offset, void*, unsigned int);
	uint64_t(*WriteOfs)(idFile* thiz, long long offset, const void*, unsigned int);
	void* (*chunky_op)(idFile*, void*, void*, void*);
	bool (*Lock)(idFile*, unsigned int, fsLock_t);
	bool (*Unlock)(idFile*, unsigned int);
	size_t(*Length)(const idFile* thiz);
	void (*SetLength)(idFile* thiz, unsigned int newlength);
	size_t(*Tell)(const idFile* thiz);
	int  (*SeekEx)(idFile* thiz, long long, fsOrigin_t origin);
	size_t(*Printf)(idFile* thiz, const char*, ...);
	size_t(*VPrintf)(idFile* thiz, const char* fmt, va_list va);
	size_t(*WriteFloatString)(idFile* thiz, const char*, ...);
	size_t(*WriteFloatStringVA)(idFile* thiz, const char* fmt, va_list va);
	size_t(*Timestamp)(const idFile* thiz);
	bool (*IsWritable)(const idFile* thiz);
	void (*Flush)(idFile*);
	void (*ForceFlush)(idFile*);
	size_t(*GetSectorSize)(const idFile*);
	int (*GetDevice)(const idFile*);
	bool (*IsOSNative)(const idFile*);
	int (*GetFileErrorCode)(const idFile*);
	void (*SetFileError)(idFile*);
	size_t(*ReadString)(idFile* thiz, idStr*);
	size_t(*ReadDebugTag)(idFile* thiz, const char*, const char*, int);
	uint64_t(*WriteDebugTag)(idFile* thiz, const char*, const char*);
};

struct idFile {
	idFileVftbl* vftbl;
};
#endif
#define	USE_FILE_LOCKING

#ifdef USE_FILE_LOCKING
#define		STDIOFN_LOCK(name, ...)		_##name##_nolock (__VA_ARGS__)
#define		EXTIOFN_LOCK(name, ...)		name##_nolock (__VA_ARGS__)
#else
#define		STDIOFN_LOCK(name, ...)		name (__VA_ARGS__)
#define		EXTIOFN_LOCK(name, ...)		name (__VA_ARGS__)
#endif
struct cs_idfile_override_t : public idFile {
	FILE* m_cfile;
	const char* m_fname;
	const char* m_fullpath;
	bool m_readable;
	bool m_writable;
	void destroy() {
		if (m_cfile) {
			
			STDIOFN_LOCK( fclose,m_cfile);
			m_cfile = nullptr;
			m_fullpath = nullptr;
			m_fname = nullptr;
			m_readable = false;
			m_writable = false;
		}
	}

	static bool return_false(const idFile* f) {
		return false;
	}

	static void deconstructor(idFile* thiz, unsigned int) {
		reinterpret_cast<cs_idfile_override_t*>(thiz)->destroy();
		idMemorySystem_free(thiz);
	}

	static const char* get_full_path(idFile* thiz) {
		return reinterpret_cast<cs_idfile_override_t*>(thiz)->m_fullpath;
	}

	static const char* get_name(idFile* thiz) {
		return reinterpret_cast<cs_idfile_override_t*>(thiz)->m_fname;
	}

	static size_t do_read(idFile* thiz, void* buffer, unsigned int len) {
		
		return STDIOFN_LOCK(fread, buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
	}
	static size_t do_write(idFile* thiz, const void* buffer, unsigned int len) {
		return STDIOFN_LOCK(fwrite, buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
	}
	static size_t do_read_offs(idFile* thiz, long long offset, void* buffer, unsigned int len) {
#if 1
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		size_t old_pos = _ftelli64(t->m_cfile);
		_fseeki64(t->m_cfile, offset, SEEK_SET);


		size_t elements = fread(buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
		_fseeki64(t->m_cfile, old_pos, SEEK_SET);
		return elements;
#else
		return doomcall<size_t>(doomoffs::_ZN13idFile_Memory7ReadOfsExPvm, thiz, offset, buffer, len);
#endif
	}
	static size_t do_write_offs(idFile* thiz, long long offset, const void* buffer, unsigned int len) {
#if 1
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		size_t old_pos = _ftelli64(t->m_cfile);
		_fseeki64(t->m_cfile, offset, SEEK_SET);


		size_t elements = fwrite(buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
		_fseeki64(t->m_cfile, old_pos, SEEK_SET);
		return elements;

#else
		return doomcall<size_t>(doomoffs::_ZN13idFile_Memory8WriteOfsExPKvm, thiz, offset, buffer, len);
#endif
	}

	static size_t do_length(const idFile* thiz) {
		auto t = reinterpret_cast<const cs_idfile_override_t*>(thiz);
		
		auto oldpos = EXTIOFN_LOCK(_ftelli64, t->m_cfile);
		EXTIOFN_LOCK(_fseeki64,t->m_cfile, 0, SEEK_END);
		auto sz = EXTIOFN_LOCK(_ftelli64, t->m_cfile);
		EXTIOFN_LOCK(_fseeki64,t->m_cfile, oldpos, SEEK_SET);
		return sz;
	}

	static void do_set_length(idFile* thiz, unsigned newl) {

	}

	static size_t do_tell(const idFile* thiz) {
		auto t = reinterpret_cast<const cs_idfile_override_t*>(thiz);
		return EXTIOFN_LOCK(_ftelli64, t->m_cfile);
	}
	static int do_seekex(idFile* thiz, long long offs, fsOrigin_t origin) {
		unsigned outseekd = 0;
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		switch (origin) {
		case FS_SEEK_CUR:
			outseekd = SEEK_CUR;
			break;
		case FS_SEEK_END:
			outseekd = SEEK_END;
			break;
		case FS_SEEK_SET:
			outseekd = SEEK_SET;
			break;
		}
		return EXTIOFN_LOCK(_fseeki64,t->m_cfile, offs, outseekd);
	}
	static size_t do_Printf(idFile* thiz, const char* fmt, ...) {
		va_list ap;
		va_start(ap, fmt);
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		size_t res = vfprintf(t->m_cfile, fmt, ap);

		va_end(ap);
		return res;
	}
	static size_t do_VPrintf(idFile* thiz, const char* fmt, va_list ap) {
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);
		return vfprintf(t->m_cfile, fmt, ap);
	}
	static size_t do_WriteFloatString(idFile* thiz, const char*, ...) {
		return 0;
	}
	static size_t do_WriteFloatStringVA(idFile* thiz, const char* fmt, va_list va) {
		return 0;
	}
	static size_t do_Timestamp(const idFile* thiz) {
		return 0;
	}
	static bool is_writeable(const idFile* thiz) {
		return reinterpret_cast<const cs_idfile_override_t*>(thiz)->m_writable;
	}

	static void do_flush(idFile* thiz) {
		STDIOFN_LOCK( fflush, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
	}
	static size_t do_GetSectorSize(const idFile*) {
		return 1;
	}
	static int do_GetDevice(const idFile*) {
		return 4;
	}

	static int do_getFileErrorCode(const idFile* thiz) {
		return ferror(reinterpret_cast<const cs_idfile_override_t*>(thiz)->m_cfile);
	}

	static void do_SetFileError(idFile* thiz) {

	}

	static size_t do_ReadString(idFile* thiz, idStr* outstr) {
#if 1
		char scratchbuffer[4096];
		unsigned buffpos = 0;
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		auto flush_buffer = [&scratchbuffer, &buffpos, outstr]() {

			if (buffpos) {
				scratchbuffer[buffpos] = 0;
				
				std::string tmpstr{};
				tmpstr = outstr->data;
				tmpstr += scratchbuffer;


				//*outstr += scratchbuffer;
				*outstr = tmpstr.c_str();
				buffpos = 0;

			}
		};


		auto add_char_to_buffer = [&scratchbuffer, &buffpos, outstr, &flush_buffer](unsigned c) {
			if (!c) {
				flush_buffer();
				return false;
			}
			else {
				scratchbuffer[buffpos++] = c;
				if (buffpos == 4095) {
					flush_buffer();
				}
				return true;
			}

		};
		size_t nread = 0;
		while (true) {
			if (feof(t->m_cfile)) {
				flush_buffer();
				break;
			}
			nread++;
			unsigned c =STDIOFN_LOCK( fgetc,t->m_cfile);

			if (!add_char_to_buffer(c))
				break;

		}

		return nread;
#else
		return doomcall<size_t>(doomoffs::_ZN6idFile10ReadStringER5idStr, thiz, outstr);
#endif
	}

	static  size_t do_ReadDebugTag(idFile* thiz, const char* a1, const char* a2, int a3) {
		//return doomcall<size_t>(doomoffs::_ZN6idFile12ReadDebugTagEPKcS1_i, thiz, a1, a2, a3);
		return 0;
	}
	static  uint64_t do_WriteDebugTag(idFile* thiz, const char*a1 , const char*a2) {
		//return doomcall<size_t>(doomoffs::_ZN6idFile13WriteDebugTagEPKcS1_, thiz, a1, a2);
		return 0;
	}

	static bool do_lock(idFile* thiz, unsigned vv, fsLock_t fl) {
#ifdef USE_FILE_LOCKING

		_lock_file(reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
#endif
		return true;
	}

	static bool do_unlock(idFile* thiz, unsigned vv) {
#ifdef USE_FILE_LOCKING

		_unlock_file(reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
#endif
		return true;
	}
	static inline idFileVftbl g_override_file_vftbl{
	.dctor = deconstructor,
	.IsIDFile_Memory = return_false,
	.IsIDFile_Permanent = return_false,
	.GetFullPath = get_full_path,
	.GetName = get_name,
	.Read = do_read,
	.Write = do_write,
	.ReadOfs = do_read_offs,
	.WriteOfs = do_write_offs,
	.Lock = do_lock,
	.Unlock = do_unlock,
	.Length = do_length,
	.SetLength = do_set_length,
	.Tell = do_tell,
	.SeekEx = do_seekex,
	.Printf = do_Printf,
	.VPrintf = do_VPrintf,
	.WriteFloatString = do_WriteFloatString,
	.WriteFloatStringVA = do_WriteFloatStringVA,
	.Timestamp = do_Timestamp,
	.IsWritable = is_writeable,
	.Flush = do_flush,
	.ForceFlush = do_flush,
	.GetSectorSize = do_GetSectorSize,
	.GetDevice = do_GetDevice,
	.IsOSNative = return_false,
	.GetFileErrorCode = do_getFileErrorCode,
	.SetFileError = do_SetFileError,
	.ReadString = do_ReadString,
	.ReadDebugTag = do_ReadDebugTag,
	.WriteDebugTag = do_WriteDebugTag


	};
	static idFile* create(FILE* fp, bool readable, bool writeable, const char* fpath, const char* name) {
		cs_idfile_override_t* overr = (cs_idfile_override_t*)idMemorySystem_malloc(sizeof(cs_idfile_override_t), 52, 0);
		overr->vftbl = &g_override_file_vftbl;
		overr->m_cfile = fp;
		overr->m_readable=readable;
		overr->m_writable = writeable;
		overr->m_fullpath = fpath;
		overr->m_fname = name;
		return overr;
	}
};




static FILE* g_readfile_log = nullptr;
extern const char* gOverrideFileName;
std::string gLastLoadedEntities;
extern std::string gOverrideName;

static void dispose_log() {
	fclose(g_readfile_log);
}
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
		if(!g_readfile_log) {
		fopen_s(&g_readfile_log, "readfile_eternal.txt", "w");
		atexit(dispose_log);
	}
	fprintf(g_readfile_log, "%s\n", name);

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

static constexpr unsigned INDEX_OF_OPENFILEREAD = 37;
static size_t readfile_repl(void* fs, const char* path, void** out_data, void* fileprops, unsigned psrch);
static void* g_old_readfile = (void*)readfile_repl;

static size_t readfile_repl(void* fs, const char* path, void** out_data, void* fileprops, unsigned psrch) {
	

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


static idFile* openfileread_replacement(void* a1, const char* a2, bool a3, int a4, int a5) {
/*	if(a5 !=FSPATH_SEARCH_BASE && a5 != FSPATH_SEARCH_DEFAULT ) {
		return g_backup_filesystem_vftbl._ZN17idFileSystemLocal12OpenFileReadEPKcbb14fsPathSearch_t(a1, a2, a3, a4, a5);
	}
	else */
	{
		size_t sz = 0;
		FILE* overr = get_override_for_resource(a2, &sz);
		if(overr) {
			return cs_idfile_override_t::create(overr, true, false, a2, a2);
		}
		else {
			return call_as<idFile*>(g_old_readfile, a1, a2, a3, a4, a5);
		}
	}
}
static void* g_old_openfileread2 = nullptr;

static idFile* openfileread2_replacement(void* a1, const char* a2, unsigned a3) {
/*	if(a5 !=FSPATH_SEARCH_BASE && a5 != FSPATH_SEARCH_DEFAULT ) {
		return g_backup_filesystem_vftbl._ZN17idFileSystemLocal12OpenFileReadEPKcbb14fsPathSearch_t(a1, a2, a3, a4, a5);
	}
	else */
	{
		size_t sz = 0;
		FILE* overr = get_override_for_resource(a2, &sz);
		if(overr) {
			return cs_idfile_override_t::create(overr, true, false, a2, a2);
		}
		else {
			return call_as<idFile*>(g_old_openfileread2, a1, a2, a3);
		}
	}
}
void hook_idfilesystem() {
	void** fs_vftbl = **reinterpret_cast<void****>(descan::g_idfilesystemlocal);

	swap_out_ptrs(&fs_vftbl[INDEX_OF_OPENFILEREAD], &g_old_readfile, 1);
	//g_old_openfileread2 = (void*)openfileread2_replacement;

//	swap_out_ptrs(&fs_vftbl[INDEX_OF_OPENFILEREAD+3], &g_old_openfileread2, 1);

}