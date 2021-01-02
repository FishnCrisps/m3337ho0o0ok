#pragma once
#include "game_exe_interface.hpp"
#include "doomoffs.hpp"

#include "scanner_core.hpp"

static inline void* idMemorySystem_malloc(size_t sz, unsigned tag, unsigned unk) {
	void*** memorysys = *reinterpret_cast<void****>(descan::g_doom_memorysystem);
	 return ((void*(__fastcall*)(void*, __int64, __int64, unsigned long long, int, int))memorysys[0][4])(memorysys, sz,
			tag,
			unk,
			16,
			-1);
}

static inline void idMemorySystem_free(void* ptr) {
	void*** memorysys = *reinterpret_cast<void****>(descan::g_doom_memorysystem);
	 return ((void(__fastcall*)(void*, void*))memorysys[0][6])(memorysys, ptr);

}