#pragma once
struct idCmdArgs {
	//offset 0 , size 4
	int argc;
	//offset 8 , size 1024
	char* argv[128];
	//offset 1032 , size 2048
	char tokenized[2048];
};

using cmdcb_t = void (*)(idCmdArgs*);
namespace idCmd {
	void register_command(const char* name, cmdcb_t, const char* description);


	void execute_command_text(const char* txt);

	void add_command(const char* txt);
	void execute_command_buffer();
}