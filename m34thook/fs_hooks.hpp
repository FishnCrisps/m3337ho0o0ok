#pragma once
#include <cstdio>


void hook_idfilesystem();


FILE* get_override_for_resource(const char* name, size_t* size_out);