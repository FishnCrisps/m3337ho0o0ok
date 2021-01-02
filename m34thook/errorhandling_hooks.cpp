#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "errorhandling_hooks.hpp"
#include <cstdio>
template<bool Fatal>
__declspec(noreturn)
static void idfatalerror_override(const char* msg, ...) {
	va_list ap;
	va_start(ap, msg);
	char tmpbuf[16384]{ 0 };
	//too lazy to check ret
	vsprintf_s(tmpbuf, msg, ap);
	MessageBoxA(nullptr, tmpbuf, Fatal ? "Engine Fatal Error" : "Engine Error", MB_ICONERROR);

	TerminateProcess(GetCurrentProcess(), 1);

}

void install_error_handling_hooks() {
	redirect_to_func((void*)idfatalerror_override<true>, (uintptr_t)descan::g_idlib_fatalerror);
	redirect_to_func((void*)idfatalerror_override<false>, (uintptr_t)descan::g_idlib_error);
}