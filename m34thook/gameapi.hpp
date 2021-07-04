#pragma once
void* find_entity(const char* name);
/*
	iterate over all entities with a given classname
	returns null when done
*/
void* find_next_entity_with_class(const char* classname, void* entity_iterator=nullptr);

void* get_level_map();

bool get_classfield_boolean(void* obj, const classVariableInfo_t* varinfo);
bool get_classfield_boolean(void* obj, const char* clazs, const char* field);
void set_classfield_boolean(void* obj, const classVariableInfo_t* varinfo, bool value);

long long get_classfield_int(void* obj, const classVariableInfo_t* varinfo);
long long get_classfield_int(void* obj, const char* clazs, const char* field);

const char* get_entity_name(void* obj);