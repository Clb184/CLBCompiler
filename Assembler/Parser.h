#pragma once
#include "Lexer.h"
#include <iostream>
#include <fstream>

struct SoundSection {
	size_t size;
	std::vector<std::string> soundArr;
	std::vector<uint32_t> soundIDs;
};

struct Music {
	std::string stringID;
	uint64_t loopStart;
	uint64_t loopEnd;
	uint32_t muscId;
};

struct MusicSection {
	size_t size;
	std::vector<Music> musicArr;
};

struct QuadSection {
	std::string vshader;
	std::string fshader;
	size_t size;
	std::vector<std::string> quadArr;
	std::vector<uint32_t> quadIDs;
};

struct Model3DSection {
	std::string vshader;
	std::string fshader;
	size_t size;
	std::vector<std::string> modelArr;
	std::vector<uint32_t> modelIDs;
};

struct ScriptFileHeader {
	std::vector<std::string> StrContainer;
	SoundSection sndm;
	MusicSection musc;
	QuadSection quad;
	Model3DSection m3dl;
};

class Parser {
public:
	Parser();
	void Initialize(const std::string&);
	bool parseTokens();
	ScriptFileHeader getScriptHeader();
	std::vector<constant> getNumArray();
private:
	bool calcAddresses();
	Lexer m_Lexer;
	std::string m_InName;
	std::map<std::string, uint32_t> m_AddressMap;
	std::map<std::string, AsmToken> m_DefinedTokenMap;
	std::vector<AsmToken> m_TokArr;
	std::vector<constant> m_NumArray;
	ScriptFileHeader m_FileHeader;
	int m_offsetEx;
};