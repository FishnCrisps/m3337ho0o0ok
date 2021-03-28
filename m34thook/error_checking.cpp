#include "error_checking.hpp"
#include <Windows.h>


void mh_run_validation() {
	if (!HeapValidate(GetProcessHeap(), 0, nullptr)) {
		//mh_raise_error("Heap is corrupted!");
	}
}