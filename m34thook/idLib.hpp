#pragma once
#include <cstdarg>

namespace idLib {
	void VPrintf(int severity, const char* fmt, va_list ap);
	static void Printf(const char* format, ... ) {
		va_list ap;
		va_start(ap, format);
		VPrintf(1, format, ap);
		va_end(ap);
	}
}