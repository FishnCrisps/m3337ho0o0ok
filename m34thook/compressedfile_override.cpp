#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "idStr.hpp"
#include <cassert>
#include "idLib.hpp"
#include "scanner_core.hpp"
#include "fs_hooks.hpp"
using _QWORD = unsigned long long;
using _BYTE = unsigned char;
using _DWORD = unsigned int;

struct idStrStatic_260_ {
	idStr base;
	//offset 48 , size 256
	char buffer[260];
};
struct idFile_Memory {
	void* vftbl;
	//offset 8 , size 312
   //Name of the file, static so we can use files across map heap boundaries without pushing heap
	idStrStatic_260_ name;
	//offset 320 , size 4
	//Open mode.
	int mode;
	//offset 328 , size 8
	//Maximum size of file.
	size_t maxSize;
	//offset 336 , size 8
	//Size of the file.
	size_t fileSize;
	//offset 344 , size 8
	//Allocated size.
	size_t allocated;
	//offset 352 , size 4
	//needed when an idFile_memory is substituted for a real file
	unsigned int timestamp;
	//offset 360 , size 8
	//Buffer holding the file data.
	char* filePtr;
	//offset 368 , size 8
	//Current read/write pointer.
	char* curPtr;
	//offset 376 , size 1
	bool ownsData;
};
static void __fastcall  sub_14033ECC0(idFile_Memory* a1, __int64 a2, __int64 a3)
{
	a1->maxSize = a3;
	a1->fileSize = a3;
	a1->allocated = a3;
	a1->mode = 0;
	a1->filePtr = (char*)a2;
	a1->curPtr = (char*)a2;
	a1->ownsData = 0;
}

void WriteOverloadMemorySize(void* Memory, size_t Size)
{
	idFile_Memory* a1 = (idFile_Memory*)Memory;
	a1->maxSize = Size;
	a1->fileSize = Size;
	a1->allocated = Size;
}

void WriteOverloadMemory(void* Memory, void* Data, size_t Size, size_t Offset)
{
	idFile_Memory* a1 = (idFile_Memory*)Memory;
	memcpy(a1->filePtr + Offset, Data, Size);
	char String[MAX_PATH];
	sprintf_s(String, "MemoryOverload: Written %llu bytes from %llX to %llX\n", Size, (__int64)Data, (__int64)a1->filePtr);
	OutputDebugStringA(String);
}

void OverloadMemory(void *Memory, void* Data, size_t Size)
{
	idFile_Memory* a1 = (idFile_Memory*)Memory;
	a1->maxSize = Size;
	a1->fileSize = Size;
	a1->allocated = Size;
	a1->mode = 0;
	memcpy(a1->filePtr, Data, Size);
	char String[MAX_PATH];
	sprintf_s(String, "MemoryOverload: Written %llu bytes from %llX to %llX\n", Size, (__int64)Data, (__int64)a1->filePtr);
	OutputDebugStringA(String);
	// a1->filePtr = (char*)a2;
	// a1->curPtr = (char*)a2;
	//(idFile_Memory*)Memory)->ownsData = 1;
}

void* __fastcall idFileResourceCompressed__GetFile(__int64 a1)
{
	__int64 v1; // rsi
	idFile_Memory* v3; // rax
	idFile_Memory* v4; // rdi
	__int64 v5; // rbp

	//the resource size
	v1 = *(_QWORD*)(a1 + 112);
	v3 = (idFile_Memory*) reinterpret_cast<void* (*)(size_t)>(descan::g_doom_operator_new)(0x180);
	//doomcall<void*>(doomoffs::doom_operator_new, 0x180);

	char* resource_name = *(char**)(a1 + 8);
	idLib::Printf("Compressed resource %s\n", resource_name);

	void*** memorysys = *reinterpret_cast<void****>(descan::g_doom_memorysystem);
		//*doomsym<void***>(doomoffs::memorySystem);

	if (v3)
		v4 = reinterpret_cast<idFile_Memory * (*)(idFile_Memory*, const char*)>(descan::g_idfilememory_ctor)(v3, resource_name);
	//doomcall<idFile_Memory*>(doomoffs::_ZN13idFile_MemoryC2EPKc, v3, resource_name);
	else
		v4 = 0i64;
	size_t override_size = 0;

	FILE* overridef = get_override_for_resource(resource_name, &override_size);
	if (overridef) {
		v5 = ((__int64(__fastcall*)(void*, __int64, __int64, _QWORD, int, int))memorysys[0][4])(memorysys, override_size,
			154i64,
			0i64,
			16,
			-1);

		fread((void*)v5, override_size, 1, overridef);
		fclose(overridef);

		//*(_QWORD*)(a1 + 112) = override_size;
		v1 = override_size;
		goto donezo;

	}

	if (*(_BYTE*)(a1 + 120))
	{


		v5 = ((__int64(__fastcall*)(void*, __int64, __int64, _QWORD, int, int))memorysys[0][4])(memorysys, v1,
			154i64,
			0i64,
			16,
			-1);
		{
			bool decompress_result = call_as<bool>(descan::g_idoodle_decompress, *(_QWORD*)(a1 + 88), *(_QWORD*)(a1 + 96), v5, v1, 0xDu);

			assert(decompress_result);
		}
	//	assert(doomcall<bool>(doomoffs::_ZN7idOodle16DecompressBufferEPKvxPvxNS_12compressor_tE, *(_QWORD*)(a1 + 88), *(_QWORD*)(a1 + 96), v5, v1, 0xDu));
	donezo:
		sub_14033ECC0(v4, v5, v1);
		v4->ownsData = 1;
	}
	else
	{
		sub_14033ECC0(v4, *(_QWORD*)(a1 + 88), v1);
	}
	return v4;
}
