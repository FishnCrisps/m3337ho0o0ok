#pragma once
struct idMapGroups {
	void* vftbl;
	//Offset 8,	 size 24
	idList < idStr , TAG_IDLIST , false > groupList;
};
struct idMapEntity : public idMapGroups {

};
struct idMapEntityLocal : public idMapEntity {
	//Offset 32,	 size 8
	idDeclEntityDef* entityDef;
	//Offset 40,	 size 4
	int entityDefLine;
	//Offset 48,	 size 80
	idMapModel model;
	//Offset 128,	 size 8
	//the parent of this reference
	idMapEntityLocal* parentReference;
	//Offset 136,	 size 48
	//reference map id, "" by default
	idStr refId;
	//Offset 184,	 size 4
	//prefab instanceId
	unsigned int instanceId;
	//Offset 192,	 size 48
	//original name if instanced from prefab
	idStr originalName;
	//Offset 240,	 size 24
	idList < idStr , TAG_IDLIST , false > layerList;
	//Offset 264,	 size 8
	idMapEntityEditorData* mapEntityEditorData;
};

struct idMapFile {
	void* vftbl;
};


struct idMapFileLocal : public idMapFile {
	//Offset 8,	 size 48
	idStr name;
	//Offset 56,	 size 4
	int version;
	//Offset 60,	 size 4
	int hierarchyVersion;
	//Offset 64,	 size 4
	unsigned int fileTime;
	//Offset 68,	 size 4
	unsigned int geometryCRC;
	//Offset 72,	 size 1
	bool hasPrimitiveData;
	//Offset 73,	 size 1
	bool isEntitiesFile;
	//Offset 80,	 size 24
	idList < idMapEntityLocal * , TAG_IDLIST , false > entities;
	//Offset 104,	 size 56
	idMapEditorStates groupStates;
	//Offset 160,	 size 24
	idList < idMapReference * , TAG_IDLIST , false > referenceMaps;
	//Offset 184,	 size 8
	idMapEntityLocal* parentReference;
	//Offset 192,	 size 48
	idStr mapModelFolder;
	//Offset 240,	 size 1
	bool expandReferences;
	//Offset 248,	 size 8
	idMapInstance* mapInstance;
	//Offset 256,	 size 1
	bool entitiesAreReference;
	//Offset 257,	 size 1
	bool ignoreGroupInfo;
	//Offset 258,	 size 1
	//If true, refmaps that are parsed can be skipped based on map properties.
	bool isRefmapSkippingAllowed;
	//Offset 264,	 size 24
	idList < idPrefabInstance * , TAG_IDLIST , false > prefabInstances;
	//Offset 288,	 size 24
	//A list of identifier-value string pairs for storing properties in the map. Properties identifiers are NOT unique.
	idList < idPair < idStr , idStr > , TAG_IDLIST , false > properties;
};
