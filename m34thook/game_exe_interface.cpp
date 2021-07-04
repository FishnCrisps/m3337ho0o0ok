#include <Windows.h>
#include <intrin.h>
#include "game_exe_interface.hpp"
static HMODULE g_reach_module = nullptr;

blamdll_t g_blamdll{};

struct patched_memory_t;

static void patch_memory_impl(void* where, char* patch_bytes, unsigned length) {
	DWORD old_prot;

	VirtualProtect(where, length, PAGE_EXECUTE_READWRITE, &old_prot);

	memcpy(where, patch_bytes, length);
	VirtualProtect(where, length, old_prot, &old_prot);
}

void patched_memory_t::undo() {
	patch_memory_impl(m_patch_location, m_original_data, m_nbytes);
}

void patched_memory_t::redo() {
	patch_memory_impl(m_patch_location, m_new_data, m_nbytes);
}

static patched_memory_t* m_patch_undo_stack_head = nullptr;


bool undo_last_patch() {
	if (m_patch_undo_stack_head == nullptr)
		return false;

	patched_memory_t* patch = m_patch_undo_stack_head;
	m_patch_undo_stack_head = patch->m_prev_patch;

	patch->undo();
	delete patch;
	return true;
}
void patch_memory(void* location, unsigned size, char* patched_data) {
	patch_memory_impl(location, patched_data, size);
}
patched_memory_t* patch_memory_with_undo(void* location, unsigned size, char* patched_data) {
	patched_memory_t* patch = new patched_memory_t(location, size, patched_data);
	patch_memory_impl(location, patched_data, size);
	patch->m_prev_patch = m_patch_undo_stack_head;
	m_patch_undo_stack_head = patch;

	return patch;
}
uintptr_t get_reach_base() {

	return reinterpret_cast<uintptr_t>(g_reach_module);
}


void init_reach() {
	if (g_reach_module == nullptr) {
		g_reach_module = GetModuleHandleA("DOOMEternalx64vk.exe");
		get_blamdll_info(&g_blamdll);

	}

}


patched_memory_t* redirect_to_func(void* hookfn, uintptr_t reachaddr, bool direct) {

	auto oldfunc = reachaddr;

	static char dabytes[] = {
	   0x48, (char)0xb8, 0,0,0,0, 0,0,0,0,0x50, (char)0xc3
	};

	*reinterpret_cast<void**>(&dabytes[2]) = hookfn;
	return patch_memory_with_undo((void*)oldfunc, sizeof(dabytes), dabytes);

}


void redirect_to_func_save_rax(void* hookfn, uintptr_t reachaddr) {
	static char dabytes[] = { 0x50, 0x48, (char)0xB8, 0x32, (char)0x89, 0x04, 0x39, (char)0xFF, (char)0x7F, 0x00, 0x00, (char)0xFF, (char)0xE0 };
	*reinterpret_cast<void**>(&dabytes[3]) = hookfn;

	patch_memory_with_undo((void*)reachaddr, sizeof(dabytes), dabytes);


}




void swap_out_ptrs(void* start_addr, void** repls, unsigned n, bool dont_want_replaced) {

	void** temp = new void* [n];

	memcpy(temp, start_addr, n * 8);

	for (unsigned i = 0; i < n; ++i) {
		void** p = temp;
		p += i;
		void* o = *p;
		*p = repls[i];
		if (!dont_want_replaced)
			repls[i] = o;
	}
	patch_memory_with_undo(start_addr, n * 8, (char*)temp);
	delete[] temp;

}


void get_blamdll_info(blamdll_t* out) {

	memset(out, 0, sizeof(blamdll_t));
	char* ntdllc = reinterpret_cast<char*>(get_reach_base());
	auto base = reinterpret_cast<IMAGE_DOS_HEADER*>(get_reach_base());
	auto winheader = reinterpret_cast<IMAGE_NT_HEADERS*>(ntdllc + base->e_lfanew);

	out->image_headers = winheader;
	out->text_base = ntdllc +
		winheader->OptionalHeader.BaseOfCode;

	out->text_size = winheader->OptionalHeader.SizeOfCode;

	auto sechead = (_IMAGE_SECTION_HEADER*)(((char*)&winheader->OptionalHeader) + sizeof(winheader->OptionalHeader));

	for (unsigned i = 0; i < winheader->FileHeader.NumberOfSections; ++i) {
		if (!strcmp((const char*)(sechead[i].Name), ".rdata")) {

			out->rdata_base = sechead[i].VirtualAddress + ntdllc;
			out->rdata_size = sechead[i].SizeOfRawData;
		}
		if (!strcmp((const char*)(sechead[i].Name), ".data")) {

			out->data_base = sechead[i].VirtualAddress + ntdllc;
			out->data_size = sechead[i].SizeOfRawData;
		}

		if(!strcmp((const char*)(sechead[i].Name), ".idata")) {
			out->idata_base = sechead[i].VirtualAddress + ntdllc;
			out->idata_size = sechead[i].SizeOfRawData;
		}

		if(!strcmp((const char*)(sechead[i].Name), ".arch")) {
			out->arch_base = sechead[i].VirtualAddress + ntdllc;
			out->arch_size = sechead[i].SizeOfRawData;			
		}
		if(!strcmp((const char*)(sechead[i].Name), ".reloc")) {
			out->reloc_base = sechead[i].VirtualAddress + ntdllc;
			out->reloc_size = sechead[i].SizeOfRawData;			
		}
	}

	if(!out->arch_base) {
		out->arch_base = out->data_base;
		out->arch_size = out->data_size;
	}
	/*
	out->rdata_base = ntdllc + winheader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress +  winheader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

	out->rdata_size = winheader->OptionalHeader.SizeOfInitializedData;

	out->data_base = out->rdata_base + out->rdata_size;
	out->data_size = winheader->OptionalHeader.SizeOfUninitializedData;
	*/
	out->image_base = (char*)get_reach_base();
	out->image_size = winheader->OptionalHeader.SizeOfImage;

}



void undo_all_reach_patches() {
	while (undo_last_patch())
		;
}

void** locate_func_in_imports(void* wantfunc) {
	char* p = g_blamdll.idata_base;

	char* end = g_blamdll.idata_size+p;

	
	while(p!= end) {
		
		if(reinterpret_cast<void**>(p)[0] == wantfunc )
			return (void**)p;
		p+=8;
	}
	return nullptr;
}
