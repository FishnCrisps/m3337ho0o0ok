#pragma once

#include <cstring>
#include <cstdint>
#include "game_exe_interface.hpp"
#include <cassert>

/*
	typical l1 size is 32k
	but we'll probably lose a chunk of that while executing
	16k gives the best performance
*/
#define		SCANNER_WORKGROUP_BLOCK_SIZE		(16*1024u)
#define		ASSERT_ALL_LOCATED
struct scanstate_t {
	unsigned char* addr;
	blamdll_t* dll;
};
enum class memsection_e {
	any,
	text,
	rdata,
	data
};

template<memsection_e section, typename TTest>
static inline bool test_section_address(blamdll_t* dll, TTest t) {
	if constexpr (section == memsection_e::any) {
		return dll->is_in_image(t);
	}
	else if constexpr (section == memsection_e::text) {
		return dll->is_in_text(t);
	}
	else if constexpr (section == memsection_e::rdata) {
		return dll->is_in_rdata(t);
	}
	else {
		return dll->is_in_data(t);
	}
}

template<unsigned char... bs>
struct scanbytes {
	static constexpr unsigned char values[] = { bs... };
	static constexpr unsigned required_valid_size = sizeof...(bs);
	static constexpr unsigned matched_size = sizeof...(bs);


	static bool match(scanstate_t& state) {

#if 1
		if constexpr (sizeof(values) == 1) {
			constexpr unsigned char cmpval = values[0];

			bool res = *state.addr == cmpval;
			state.addr += 1;

			return res;
		}
		else if constexpr (sizeof(values) == 2) {
			constexpr unsigned short cmpval = (((unsigned short)values[1]) << 8) | ((unsigned short)values[0]);

			bool res = *(unsigned short*)state.addr == cmpval;
			state.addr += 2;

			return res;
		}
		else if constexpr (sizeof(values) == 3) {
			constexpr unsigned short cmpval1 = (((unsigned short)values[1]) << 8) | ((unsigned short)values[0]);
			constexpr unsigned char cmpval2 = values[2];

			bool res = (*(unsigned short*)state.addr == cmpval1) & (cmpval2 == state.addr[2]);


			state.addr += 3;
			return res;
		}
		else if constexpr (sizeof(values) == 4) {
			constexpr unsigned cmpval = ((unsigned)values[0]) | (((unsigned)values[1]) << 8) |
				(((unsigned)values[2]) << 16) | (((unsigned)values[3]) << 24);

			bool res = *((unsigned*)state.addr) == cmpval;
			state.addr += 4;
			return res;
		}
		else {
			if (!memcmp((const void*)state.addr, (const void*)values, sizeof...(bs))) {
				state.addr += sizeof...(bs);
				return true;
			}
		}
#else
		if (!memcmp((const void*)state.addr, (const void*)values, sizeof...(bs))) {
			state.addr += sizeof...(bs);
			return true;
		}
#endif
		return false;
	}

};

template<void** to>
struct match_riprel32_to {

	static constexpr unsigned required_valid_size = 4;


	static bool match(scanstate_t& state) {
		if (!state.dll->is_in_image(state.addr))
			return false;
		ptrdiff_t df = *reinterpret_cast<int*>(state.addr);

		if (*to != (void*)(state.addr + 4 + df)) {
			return false;
		}

		state.addr += 4;
		return true;
	}
};

template<unsigned char... data>
struct riprel32_data_equals {
	static constexpr unsigned char values[] = { data... };

	static constexpr unsigned required_valid_size = 4;
	static bool match(scanstate_t& state) {
		unsigned char* base = state.addr + 4;

		signed displ = *(signed*)state.addr;
		base += displ;

		if ((char*)base < state.dll->image_base || (char*)base >(state.dll->image_base + state.dll->image_size))
			return false;
		state.addr += 4;

		return !memcmp((const void*)base, (const void*)(&values[0]), sizeof...(data));
	}

};

template<typename recursive_pat>
struct match_calltarget_riprel32_recursive {
	static constexpr unsigned required_valid_size = 4;
	static bool match(scanstate_t& state) {

		unsigned char* base = state.addr + 4;

		signed displ = *(signed*)state.addr;
		base += displ;


		if ((char*)base < state.dll->text_base || (char*)base >(state.dll->text_base + state.dll->text_size))
			return false;

		scanstate_t recurse_state{ base, state.dll };


		bool result = recursive_pat::match(recurse_state);

		state.addr += 4;
		return result;
	}
};

template<unsigned N>
struct skip {
	static constexpr unsigned required_valid_size = N;
	static bool match(scanstate_t& state) {
		state.addr += N;
		return true;
	}
};

template<void** rva_out>
struct skip_and_capture_rva {
	//static inline T value{};
	static constexpr unsigned required_valid_size = 4;
	static bool match(scanstate_t& state) {


		ptrdiff_t df = *reinterpret_cast<int*>(state.addr);

		*rva_out = (void*)(state.addr + 4 + df);

		state.addr += 4;
		return true;
	}
};

template<unsigned n>
struct align_next {
	static constexpr unsigned required_valid_size = n;
	static bool match(scanstate_t& state) {
		state.addr += (n - 1);
		state.addr = (unsigned char*)(((uintptr_t)state.addr) & ~(uintptr_t)(n - 1));

		return true;
	}
};



enum memscanner_flags_e {
	_test_mapped_displ_in_image = 1,

};
template<uint64_t scanner_flags>
struct memscanner_factory_t {
	template<typename... Ts>
	struct memscanner_t {

		template<typename TCurr, typename... TRest>
		static bool test_addr(scanstate_t& state) {

			if (!TCurr::match(state)) {
				return false;
			}

			if constexpr (sizeof...(TRest) != 0) {
				return test_addr<TRest...>(state);
			}
			return true;

		}
		template<typename TCurr, typename... TRest>
		static constexpr unsigned compute_overall_required_mapped_bytes() {
			unsigned v = TCurr::required_valid_size;
			if constexpr (sizeof...(TRest)) {
				return v + compute_overall_required_mapped_bytes<TRest...>();
			}
			return v;
		}

		static constexpr unsigned required_mapped_bytes = compute_overall_required_mapped_bytes<Ts...>();


		static bool match(scanstate_t& state) {
			if constexpr (scanner_flags & _test_mapped_displ_in_image) {
				if (!state.dll->is_in_image(state.addr + required_mapped_bytes))
					return false;
			}
			return test_addr<Ts...>(state);
		}
	};
};

using workgroup_result_t = void*;


using worker_match_behavior_t = workgroup_result_t(*)(unsigned displacement);

template<typename... Ts>
using memscanner_t = typename memscanner_factory_t<0>::template  memscanner_t<Ts...>;

template<uint64_t flags, typename... Ts>
using memscanner_with_flags_t = typename memscanner_factory_t<flags>::template memscanner_t<Ts...>;




template<typename T>
static inline void* run_simple_scanner() {



	for (unsigned i = 0; i < g_blamdll.text_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.text_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)(reinterpret_cast<unsigned*>(scan.addr)[0] + get_reach_base());

		}
	}
	if (g_blamdll.reloc_base) {

		for (unsigned i = 0; i < g_blamdll.reloc_size; ++i) {
			scanstate_t scan{ (unsigned char*)(i + g_blamdll.reloc_base), &g_blamdll };

			if (T::match(scan)) {

				return (void*)(reinterpret_cast<unsigned*>(scan.addr)[0] + get_reach_base());

			}
		}
	}

	return nullptr;

}

template<typename T>
static inline void* run_simple_data_scanner() {



	for (unsigned i = 0; i < g_blamdll.data_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.data_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)scan.addr;

		}
	}
	return nullptr;

}
template<typename T>
static inline void* run_simple_image_scanner() {



	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)scan.addr;

		}
	}
	return nullptr;

}

template<typename T>
static inline void* locate_func() {


	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)(i + g_blamdll.image_base);

		}
	}
	return nullptr;

}
template<typename T>
static inline void* locate_rdata_ptr_to() {


	for (unsigned i = 0; i < g_blamdll.rdata_size; i += 8) {

		char* p = *(char**)(i + g_blamdll.rdata_base);
		if (p >= g_blamdll.text_base && p < (g_blamdll.text_base + g_blamdll.text_size)) {

			scanstate_t scan{ (unsigned char*)(p), &g_blamdll };

			if (T::match(scan)) {

				return (void*)(i + g_blamdll.rdata_base);

			}
		}
	}
	return nullptr;
}

template<typename T>
static inline void* locate_csrelative_address_preceding() {

	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {



			auto displ = *(signed*)((unsigned char*)(i + g_blamdll.image_base) - 4);

			return (void*)(i + g_blamdll.image_base + displ);

		}
	}
	return nullptr;
}

/*
	for extracting trivial hs function calls
*/
template<typename T>
static inline void* extract_csrelative_call_address_before_first_retn_in_func(void* func_base) {
	char* fn = (char*)func_base;
	for (unsigned i = 0; fn[i] != 0xC3; ++i) {
		scanstate_t scan{ (unsigned char*)(&fn[i]), &g_blamdll };

		if (T::match(scan)) {


			auto displ = *(signed*)((unsigned char*)(&fn[i]) - 4);


			return (void*)(i + fn + displ);

		}
	}

}
template<typename T>
static inline void* extract_csrelative_call_address_before_first_retn_in_func_last4bytes(void* func_base) {

	char* fn = (char*)func_base;
	for (unsigned i = 0; fn[i] != 0xC3; ++i) {
		scanstate_t scan{ (unsigned char*)(&fn[i]), &g_blamdll };

		if (T::match(scan)) {

			return (void*)(*reinterpret_cast<signed*>(scan.addr - 4) + scan.addr);
		}
	}
	return nullptr;
}

template<typename T>
void* locate_csrelative_address_4bytes_before_end() {



	for (unsigned i = 0; i < g_blamdll.text_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.text_base), &g_blamdll };

		if (T::match(scan)) {
			return (void*)(*reinterpret_cast<signed*>(scan.addr - 4) + scan.addr);
		}
	}
	return nullptr;
}


template<typename T, memsection_e constrain = memsection_e::any>
static inline void* locate_csrelative_address_after() {


	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {



			auto displ = (scan.addr + 4) + *(signed*)scan.addr;

			if (!test_section_address<constrain>(&g_blamdll, displ)) {
				continue;
			}

			return (void*)(displ);

		}
	}
	return nullptr;
}


template<typename T>
static workgroup_result_t scanbehavior_simple(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {

		return (void*)scan.addr;

	}
	return nullptr;
}
template<typename T>
static workgroup_result_t scanbehavior_locate_func(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {

		return (void*)(i + g_blamdll.image_base);

	}
	return nullptr;
}
template<typename T>
static workgroup_result_t scanbehavior_locate_csrel_preceding(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {



		auto displ = *(signed*)((unsigned char*)(i + g_blamdll.image_base) - 4);

		return (void*)(i + g_blamdll.image_base + displ);

	}
	return nullptr;
}
template<typename T>
static workgroup_result_t scanbehavior_locate_csrel_after(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {



		auto displ = (scan.addr + 4) + *(signed*)scan.addr;

		if (!test_section_address<memsection_e::any>(&g_blamdll, displ)) {
			return nullptr;
		}

		return (void*)(displ);

	}
	return nullptr;
}
struct scangroup_listnode_t {
	struct block_scangroup_entry_t* m_next;
	struct block_scangroup_entry_t* m_prev;

	scangroup_listnode_t() : m_next(nullptr), m_prev(nullptr) {}
};

struct block_scangroup_entry_t : public scangroup_listnode_t {



	workgroup_result_t* m_result_receiver;

	virtual workgroup_result_t execute_on_block(unsigned displ) = 0;
	//same as execute_on_block, but prefetches for the next block
	virtual workgroup_result_t execute_on_block_prefetching(unsigned displ) = 0;
	template<bool prefetching = false>
	block_scangroup_entry_t* run(unsigned i) {

		workgroup_result_t res = prefetching ?
			execute_on_block_prefetching(i) :
			execute_on_block(i);
		block_scangroup_entry_t* nextscanner = m_next;

		if (res) {
			/*
				we're done, remove us from the execution list
			*/
			*m_result_receiver = res;
			if (m_prev) {
				m_prev->m_next = m_next;
			}

			if (m_next) {
				m_next->m_prev = m_prev;
			}
		}
		return nextscanner;
	}
	block_scangroup_entry_t() : scangroup_listnode_t() {}

};
//extradata is in case we need to add more stuff here
template<
	workgroup_result_t* out_result_global,
	worker_match_behavior_t behavior,
	typename Extradata = void>
	struct blockscan_entry_definition_t {


	struct implementer_t : public block_scangroup_entry_t {
		implementer_t() : block_scangroup_entry_t() {
			m_result_receiver = out_result_global;

		}

		virtual workgroup_result_t execute_on_block(unsigned displ) override {

			unsigned end = displ + SCANNER_WORKGROUP_BLOCK_SIZE;
			end = end > g_blamdll.image_size ? g_blamdll.image_size : end;

			for (unsigned i = displ; i < end; ++i) {
				void* res = behavior(i);
				if (res)
					return res;

				//return behavior(displ);
			}
			return nullptr;
		}
		virtual workgroup_result_t execute_on_block_prefetching(unsigned displ) override {



			unsigned end = displ + SCANNER_WORKGROUP_BLOCK_SIZE;
			end = end > g_blamdll.image_size ? g_blamdll.image_size : end;


			unsigned prefetchiters = (end - displ) / 64;

			for (unsigned iiter = 0; iiter < prefetchiters; ++iiter) {
				_mm_prefetch((iiter * 64) +displ + SCANNER_WORKGROUP_BLOCK_SIZE + g_blamdll.image_base, _MM_HINT_T0);

				for (unsigned j = 0; j < 64; ++j) {
					void* res = behavior(((iiter*64)+j)+displ);
					if (res)
						return res;
				}
				/*
					we're ending our usage of this block so flush it from the cache to make room for the next block
				*/
				_mm_clflush(((iiter*64) + displ) + g_blamdll.image_base);
			}

			
			//handle modulo cachesize 
			for (unsigned i = (prefetchiters*64) + displ ; i < end; ++i) {
				//if (!(i & 63))
			//		_mm_prefetch(i + SCANNER_WORKGROUP_BLOCK_SIZE + g_blamdll.image_base, _MM_HINT_T1);

				void* res = behavior(i);
				if (res)
					return res;

				//return behavior(displ);
			}
			return nullptr;
		}
	};

	static inline implementer_t g_blockscan_node{};

};


template<typename... Ts>
struct scangroup_t {
	static inline scangroup_listnode_t m_list;
	template<typename TCurr, typename... TNext>
	static void link_group(scangroup_listnode_t* prevnode) {

		prevnode->m_next = &TCurr::g_blockscan_node;

		TCurr::g_blockscan_node.m_prev = (block_scangroup_entry_t*)prevnode;

		if constexpr (sizeof...(TNext)) {
			link_group<TNext...>(&TCurr::g_blockscan_node);
		}

	}


	scangroup_t() {
		link_group<Ts...>(&m_list);
	}
#ifdef ASSERT_ALL_LOCATED
	template<typename T, typename... TRest>
	static void assert_all_located() {
		assert(*(T::g_blockscan_node.m_result_receiver));

		if constexpr (sizeof...(TRest)) {
			assert_all_located<TRest...>();
		}
	}
#endif
	static void execute_on_image() {

		unsigned i = 0;

		unsigned end = g_blamdll.image_size;

		for (; i + SCANNER_WORKGROUP_BLOCK_SIZE < end; i += SCANNER_WORKGROUP_BLOCK_SIZE) {

			
			if (!m_list.m_next)
				return;
			block_scangroup_entry_t* entry = m_list.m_next;


			while (entry->m_next) {
				//		if(!entry->m_next)
					//		entry->run<true>(i);
					//	else
				entry = entry->run(i);

			}
			entry->run<true>(i);

		}

#ifdef ASSERT_ALL_LOCATED
		assert_all_located<Ts...>();
#endif
	}

};

/*
	execute a memscanner on a small window of the program
*/
template<worker_match_behavior_t ScanBehavior>
__forceinline static void* scan_function_boundaries(void* func, unsigned assumed_size) {
	unsigned fdelta = (char*)func - g_blamdll.image_base;


	unsigned fend = fdelta + assumed_size;
	for (; fdelta < fend; ++fdelta) {
		void* res = ScanBehavior(fdelta);
		if (res)
			return res;
	}
	return nullptr;
}
template<worker_match_behavior_t ScanBehavior>
__forceinline static void* scan_guessed_function_boundaries(void* func) {
	unsigned char* pf = (unsigned char*)func;
	/*
		while in range of image and not breakpoint (in between align16 for functions) and not at ret
	*/
	while (pf >= (unsigned char*)g_blamdll.image_base && pf < (unsigned char*)(g_blamdll.image_base + g_blamdll.image_size) && *pf != 0xCC && *pf != 0xC3) {
		++pf;
	}

	unsigned assumed_size = (char*)pf - (char*)func;
	return scan_function_boundaries<ScanBehavior>(func, assumed_size);
}
__forceinline static void* hunt_assumed_func_start_back(void* func) {
	unsigned char* pf = (unsigned char*)func;
	/*
		while in range of image and not breakpoint (in between align16 for functions) and not at ret
	*/
	while ( (*pf != 0xCC && *pf != 0xC3) || (reinterpret_cast<uintptr_t>(pf + 1)&0xF)) {
		--pf;
	}



	return pf + 1;
}