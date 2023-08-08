#pragma once
#include "Parser.h"

class Preprocessor {
public:
	bool initPreprocessor(std::vector<AsmToken>&, std::vector<AsmToken>&, ScriptFileHeader&);
	void appendImport(std::string, size_t, std::vector<AsmToken>&);
private:
	std::map<std::string, AsmToken> m_DefinedTokenMap;
	Lexer m_InternalLexer;
};