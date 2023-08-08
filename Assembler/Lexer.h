#pragma once
#include <vector>
#include <string>
#include <map>
#include "..\..\Engine\Commons.h"
#include "..\..\Engine\Utyls\FileIO\TextFile.h"
#include "..\..\Engine\ScriptBase.h"

typedef enum {
	UNKNOWN2 = -1,
	KEYWORD2 = 0,
	IDENTIFIER2,
	REAL_LIT2,
	INT_LIT2,
	STRING_LIT2,
	SEPARATOR2,
	ADDRESS = 6,
	PREPROCESSOR2,
	EOSCRIPT2,
	WHITESPACE2,
	RAD2,
	IGNORE2,
} ASMTOKEN;

typedef enum {
	ASM_SOUND,
	ASM_MUSIC,
	ASM_SHADER2D,
	ASM_SHADER3D,
	ASM_QUAD,
	ASM_MODEL3D,
	ASM_TEXT,
	ASM_CONSTANT,
	ASM_IMPORT,
} ASMPREPROCESOR;

struct AsmToken {
	ASMTOKEN tok;
	std::string val;
	constant numval;
	size_t line;
};

class Lexer {
public:
	Lexer();
	void Initialize(std::string);
	bool tokenizeSrc();
	void initValues();
	std::vector<AsmToken> getTokens();
private:
	AsmToken tokenizeItem(std::string&, size_t);
	size_t m_Line;
	std::vector<AsmToken> m_TokenArr;
	std::string m_Src;
	std::string m_Input;
	std::map<std::string, ScriptCommand> m_CommandMap;
	std::map<std::string, ASMPREPROCESOR> m_PrepropMap;
};