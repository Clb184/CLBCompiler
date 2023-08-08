#pragma once
#include "Parser.h"

class Preprocessor {
public:
	bool initPreprocessor( std::vector<AsmToken>&, ScriptFileHeader&, std::string);
private:
	bool appendImport(std::string, size_t&, std::vector<AsmToken>&, ScriptFileHeader&);
	std::string retrievePath(std::string, std::string);
private:
	std::map<std::string, AsmToken> m_DefinedTokenMap;
	std::map<std::string, bool> m_IncludedFiles;
};