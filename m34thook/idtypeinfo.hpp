#pragma once
#pragma pack(push, 1)

struct enumValueInfo_t {
	//offset 0 , size 8
	char* name;
	//offset 8 , size 4
	long long value;

};
struct enumTypeInfo_t {
	//offset 0 , size 8
	char* name;
	//offset 8 , size 4
	unsigned long long flags;
	int type;
	int valueIndexLength;

	//offset 16 , size 8
	enumValueInfo_t* values;
	int* valueIndex;
};
struct classMetaDataInfo_t
{
  char *metaData;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct classVariableInfo_t
{
	char* type;
	char* ops;
	char* name;
	int offset;
	int size;
	int flags;
	char pad36[4];
	char* comment;
	int (*get)(void* ptr);
	//offset 56 , size 8
	void (*set)(void* ptr, int value);
	//offset 64 , size 8
	void* (*reallocate)(void* ptr, const int oldNum, const int newNum, const int tag, const bool zeroBuffer);

};
#pragma pack(pop)
struct idClass;
struct idRenderModel;
struct idDeclModelAsset;

#pragma pack(push, 1)
struct classTypeInfo_t
{
	char* name;
	char* superType;
	int size;
	char pad20[4];
	classVariableInfo_t* templateParms;
	classVariableInfo_t* variables;
	unsigned long long* variableNameHashes;

	idClass* (*createInstance)();
	idRenderModel* (*createModel)(const idDeclModelAsset * modelAsset , const unsigned int createFlags);
	classMetaDataInfo_t* metaData;
};
#pragma pack(pop)


namespace idType {
	
	classTypeInfo_t* FindClassInfo(const char* cname);
	enumTypeInfo_t* FindEnumInfo(const char* enumname);
	classVariableInfo_t* FindClassField(const char* cname, const char* fieldname);
	void do_idlib_dump();
}