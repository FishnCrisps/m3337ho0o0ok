#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "idLib.hpp"
#include "scanner_core.hpp"
void idLib::VPrintf(int severity, const char* fmt, va_list ap) {
//	doomcall<void>(doomoffs::_ZN5idLib7VPrintfE15printSeverity_tPKcSt9__va_list,1, severity, fmt, ap);
	call_as<void>(descan::g_idlib_vprintf,1, severity, fmt, ap);
}