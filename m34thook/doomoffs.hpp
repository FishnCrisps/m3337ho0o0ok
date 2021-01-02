#pragma once

namespace doomoffs {
	static constexpr auto cmdSystem = 0x3b30628;
	static constexpr auto gamelib_init_ptr = 0x335b090;
	static constexpr auto gamelib_initialize = 0xacb3e0;
	static constexpr auto global_Typeinfo_tools = 0x3f95698;
	static constexpr auto _ZNK15idTypeInfoTools13FindClassInfoEPKcS1_ = 0x3c7450;
	//idStr::idStr()
	static constexpr auto _ZN5idStrC2Ev = 0x3b0fe0;
	//idStr::idStr(const char*)
	static constexpr auto _ZN5idStrC2EPKc = 0x3b0ee0;
	//idstr dctor
	static constexpr auto _ZN5idStrD2Ev = 0x3b1180;
	//idstr operator = (const char*)
	static constexpr auto _ZN5idStraSEPKc = 0x3b14f0;

	static constexpr auto eventdef_interface = 0x4b25468;
	static constexpr auto _ZNK15idTypeInfoTools12FindEnumInfoEPKc = 0x3c87d0;
	static constexpr auto _ZN5idLib7VPrintfE15printSeverity_tPKcSt9__va_list = 0x324d00;
	static constexpr auto idFileResourceCompressed__GetFile = 0x8d50b0;
	static constexpr auto memorySystem = 0x3b30618;
	static constexpr auto _ZN13idFile_MemoryC2EPKc = 0x33c2c0;
	static constexpr auto _ZN7idOodle16DecompressBufferEPKvxPvxNS_12compressor_tE = 0x32d6a0;
	static constexpr auto doom_operator_new = 0x319f70;
}