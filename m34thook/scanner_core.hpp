#pragma once


using located_feature_t = void*;
//need 1080 getlevelmap


namespace descan {
	//alloca probe function, for matching calls to it
	extern located_feature_t g_alloca_probe;
	extern located_feature_t g_security_cookie;
	extern located_feature_t g_security_check_cookie;
	extern located_feature_t g_msvc_typeinfo_vtbl;
	extern located_feature_t g_doom_memorysystem;
	extern located_feature_t g_sqrtf, g_sqrt;
	extern located_feature_t g_idfilecompressed_getfile;
	extern located_feature_t g_doom_operator_new;
	extern located_feature_t g_idfilememory_ctor;
	extern located_feature_t g_idoodle_decompress;
	extern located_feature_t g_idcmdsystem, g_idcvarsystem, g_idlib_vprintf, g_idstr_hash;

	extern located_feature_t g_idtypeinfo_findclassinfo, g_idtypeinfo_findenuminfo;

	extern located_feature_t g_gamelib_initialize_ptr;

	extern located_feature_t g_idstr__idstr;
	extern located_feature_t g_idTypeInfoFile_readstr;
	extern located_feature_t g_resourcelist_for_classname;
	extern located_feature_t g__ZN5idStr4IcmpEPKcS1_;
	extern located_feature_t g_listOfResourceLists;
	extern located_feature_t g_gamelocal;
	extern located_feature_t g_idgamesystemlocal;
	extern located_feature_t g_idgamesystemlocal_find;
	extern located_feature_t g_command_patch_area;
	extern located_feature_t g_serialize_type_to_json;
	extern located_feature_t g_deserialize_type_from_json;
	extern located_feature_t g_atomic_string_set;

	extern located_feature_t g_declentitydef_setentitystate;
	extern located_feature_t g_declentitydef_gettextwithinheritance;
	extern located_feature_t g_idlib_warning, g_idstr_dctor, g_idstr_assign_charptr, g_noclip_func, g_global_typeinfo_tools;
	extern located_feature_t g_maplocal_getlevelmap, g_idmapfile_write;//

	extern located_feature_t g_idlib_fatalerror;
	extern located_feature_t g_idlib_error;

	extern located_feature_t g_idfilesystemlocal;
	extern located_feature_t g_levelreload;
#include "declare_vtbl_feature_vars.hpp"


	void locate_critical_features();
}