#include "cmdsystem.hpp"
#include "game_exe_interface.hpp"
//#include "doomoffs.hpp"
#include "scanner_core.hpp"
void idCmd::register_command(const char* name, cmdcb_t cb, const char* description) {
	//auto cmdSystem = *doomsym<char**>(doomoffs::cmdSystem);

	auto cmdSystem = *(char**)descan::g_idcmdsystem;
	(*(void(__fastcall**)(__int64, const char*, cmdcb_t, const char*, unsigned long long, unsigned))(*(unsigned long long*)cmdSystem + 24i64))(
		(__int64)cmdSystem,
		name,
		cb,
		description,
		0i64,
		0);

}
struct idCmdSystemLocal;


using _QWORD = unsigned  long long;
struct idCmdSystemLocalVftbl
{
  void* first; // 0
  void *field_8; // 8
  __int64 (__fastcall *field_10)(__int64 a1, int a2); // 10
  void *_ZN16idCmdSystemLocal10AddCommandEPKcPFvRK9idCmdArgsES1_PFvR14idAutoCompleteEi; // 18
  const char *(__fastcall *_ZN16idCmdSystemLocal21GetCommandDescriptionEPKc)(__int64 a1, __int64 a2); // 20
  _QWORD (__fastcall *_ZN16idCmdSystemLocal13CommandExistsEPKcb)(idCmdSystemLocal * thiz, const char *, bool); // 28
  void *_ZN16idCmdSystemLocal12FindCommandsEPKcR6idListI5idStrL8memTag_t5ELb0EE; // 30
  void *field_38; // 38 
  _QWORD (__fastcall *_ZN16idCmdSystemLocal18ExecuteCommandTextEPKc)(idCmdSystemLocal * thiz, const char *); // 40
  void *field_48;
  //void *field_50;
  _QWORD(__fastcall* _AppendCommandHack)(idCmdSystemLocal* thiz, const char*); // 48
  void *_GenerateTestCases__$TestDummyTemplatedParametricTestFixture_V_$hash_map_H_NU_$hash_H_core__U_$equal_to_H_std___core___SuiteTemplatedTestkUnitTestCategory__SAXAEAU_$TestCaseEmitter_HXXXX_Testing___Z;
  void *field_60;
  __int64 (__fastcall  *field_68)(_QWORD *a1);
  __int64 (__fastcall *field_70)(_QWORD *a1);
  _QWORD (*_ZN16idCmdSystemLocal20ExecuteCommandBufferEv)(idCmdSystemLocal * thiz);
  void *field_80;
};


struct idCmdSystemLocal {
	idCmdSystemLocalVftbl* vftbl;
};

static idCmdSystemLocal* cmdSystem_get() {
	return *reinterpret_cast<idCmdSystemLocal**>(descan::g_idcmdsystem);
}

void idCmd::execute_command_text(const char* txt) {
	auto cmdsys = cmdSystem_get();
	cmdsys->vftbl->_ZN16idCmdSystemLocal18ExecuteCommandTextEPKc(cmdsys, txt);
}

void idCmd::add_command(const char* txt)
{
	auto cmdsys = cmdSystem_get();
	cmdsys->vftbl->_AppendCommandHack(cmdsys, txt);
}

void idCmd::execute_command_buffer() {
	auto cmdsys = cmdSystem_get();
	cmdsys->vftbl->_ZN16idCmdSystemLocal20ExecuteCommandBufferEv(cmdsys);
}