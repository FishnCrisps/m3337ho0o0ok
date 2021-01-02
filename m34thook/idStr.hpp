#pragma once
#include "game_exe_interface.hpp"
#include "scanner_core.hpp"
struct idStr {
	void* vftbl;
	char* data;
	int len;
	//top bit is used to store a flag that indicates if the string data is static or not
	int allocedAndFlag;
	char baseBuffer[20];


	idStr() {
		call_as<void>(descan::g_idstr__idstr, this);
		//doomcall<void>(doomoffs::_ZN5idStrC2Ev, this);
	}
	

	~idStr() {
		call_as<void>(descan::g_idstr_dctor, this);
	}

	idStr& operator = (const char* s) {
		call_as<void>(descan::g_idstr_assign_charptr, this, s);
		return *this;
	}
};

struct idAtomicString
{
  char *str;
};
