#pragma once
struct resourceList_t;
struct idResourceList {
	resourceList_t* resourceList;
	char* typeName;
	char* className;
	idResourceList* next;
	void (*postInitCallback)(idResourceList * const resourceList);
};