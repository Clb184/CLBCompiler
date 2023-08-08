#pragma once
#include <fstream>
#include "Parser.h"

class Compiler {
public:
	Compiler();
	void Initialize(const std::string&, const std::string&);
	void compileFile();
private:
	size_t calculateSize();
	void writeBuffer(char*);
	Parser m_Parser;
	std::string m_OutName;
	std::ofstream m_Output;
	std::vector<std::string> m_GlobalStrContainer;
	ScriptFileHeader m_Header;
	std::vector<int> m_HeaderSub;
	std::vector<constant> m_NumArr;
	std::map<std::string, int> m_stringMap;
};