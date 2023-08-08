#include "FileReader.h"
#include "Assembler/Compiler.h"
#include <iostream>

#ifdef _DEBUG
	int main() {
		ScriptCompiler scpt;
		Compiler comp;
		std::string input = "D:\\repos\\BasicEngine\\Engine\\resource\\script_src\\main_title.xasm";
		std::string output = "D:\\repos\\BasicEngine\\Engine\\resource\\script_src\\asmtest_deb.bin";
		std::cout << "Source input: " << input << "\n"; //std::cin >> input;
		//scpt.Initialize(input, "dummy.bin");
		comp.Initialize(input, output);
		comp.compileFile();
	}
#else
	int main(int argc, char* args[]) {
		if (argc == 3) {
			Compiler comp;
			std::string input = args[1];
			std::string output = args[2];
			comp.Initialize(input, output);
			comp.compileFile();
		}
		return 0;
	}
#endif