#pragma once
#include <stdarg.h>

/*
copy of idcmdargs
*/
typedef struct  {
	int argc;
	char* argv[128];
	char tokenized[2048];
}mh_cmd_args_t;
/*
	get entitydef pointer from entity using offset in classinfo

	findentity seems to be the same offset in all versions
*/
typedef struct  {
	void (*register_command)(void (*cmd)(mh_cmd_args_t* args));
	void (*execute_console_command)(const char* command_text);

	void (*console_vprintf)(const char* fmt, va_list ap);

	void (*set_string_atomic)(char** s, const char* value);

}mh_api_t;