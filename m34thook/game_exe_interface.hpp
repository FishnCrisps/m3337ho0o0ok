#pragma once
#include <Windows.h>
#include <cstddef>
struct blamdll_t {
    char* text_base;
    char* rdata_base;
    char* data_base;
    unsigned text_size;
    unsigned rdata_size;
    unsigned data_size;
    unsigned idata_size;
    unsigned arch_size;
    unsigned reloc_size;
    void* tls_section;
    char* image_base;
    char* idata_base;
    char* arch_base;
    char* reloc_base;
    unsigned image_size;
    IMAGE_NT_HEADERS* image_headers;
    template<typename T>
    bool is_in_text(T v) {
        return (char*)v >= text_base && (char*)v < (text_base+text_size);
    }

    template<typename T>
    bool is_in_rdata(T v) {
        return (char*)v >= rdata_base && (char*)v < (rdata_base+rdata_size);
    }
    template<typename T>
    bool is_in_data(T v) {
        return (char*)v >= data_base && (char*)v < (data_base+data_size);
    }

    template<typename T>
    bool is_in_image(T v) {
        return (char*)v >= image_base && (char*)v < (image_base+image_size);
    }
};


struct patched_memory_t {
    char* m_original_data;
    char* m_new_data;
    struct patched_memory_t* m_prev_patch;
    size_t m_nbytes;
    void* m_patch_location;

    patched_memory_t(void* location, unsigned size, void* newdata) {
        m_original_data = new char[size];
        m_new_data = new char[size];
        memcpy(m_original_data, location, size);
        memcpy(m_new_data, newdata, size);
        m_prev_patch = nullptr;
        m_nbytes = size;
        m_patch_location = location;
    }
    ~patched_memory_t() {
        delete[] m_original_data;
        // delete m_prev_patch;
    }

    void undo();
    void redo();
};

void init_reach();

uintptr_t get_reach_base();
patched_memory_t* patch_memory_with_undo(void* location, unsigned size, char* patched_data);
void patch_memory(void* location, unsigned size, char* patched_data);

patched_memory_t* redirect_to_func(void* hookfn, uintptr_t reachaddr,bool direct=false);


void get_blamdll_info(blamdll_t* out);


void redirect_to_func_save_rax(void* hookfn, uintptr_t reachaddr);


void swap_out_ptrs(void* start_addr, void** repls, unsigned n, bool dont_want_replaced=false);


void undo_all_reach_patches();

/*
    finds the address in the import section of a pointer to wantfunc
    or returns null
*/
void** locate_func_in_imports(void* wantfunc);

extern blamdll_t g_blamdll;
void** locate_import_ptr(const char* impname);

template<typename T>
static inline T* doomsym(uintptr_t addr) {
	return reinterpret_cast<T*>(g_blamdll.image_base + addr);
}

template<typename TRet, typename... Ts>
static inline TRet doomcall(uintptr_t addr, Ts... args) {
	return doomsym<TRet (Ts...)>(addr)(args...);
}

template<typename TRet, typename... Ts>
static inline TRet call_as(void* addr, Ts... args) {
	return reinterpret_cast<TRet (*)(Ts...)>(addr)(args...);
}

struct rtti_obj_locator_t {
    int unused[3];
    int typedescr_rva;
    int hierarchy_descr_rva;
    int objbase_rva;
};