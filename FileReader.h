#pragma once
//#include "..\BasicEngine\Commons.h"
//#include "..\BasicEngine\ScriptBase.h"
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <map>
#include <stack>
#include <vector>
#include <string>
#include <string.h>
#include "..\Engine\Utyls\FileIO\TextFile.h"
#include "..\Engine\Commons.h"

typedef enum {
	UNKNOWN = -1,
	KEYWORD = 0,
	IDENTIFIER,
	REAL_LIT,
	INT_LIT,
	STRING_LIT,
	SEPARATOR,
	ARITMETHIC,
	PREPROCESSOR,
	EOSCRIPT,
	WHITESPACE,
	SCOPE,
	IGNORE,
} TOKEN;

typedef enum {
	KEY_INVALID = -1,
	KEY_VOID = 0,
	SUB = 0,
	IF,
	ELSE,
	SWITCH,
	FOR,
	WHILE,
	BREAK,
	RETURN,
} COMPKEYWORD;


typedef enum {
	SCOPE_INVALID = -1,
	SCOPE_VOID = 0,
	SCOPE_IF,
	SCOPE_ELSE,
	SCOPE_ELSE_IF,
	SCOPE_SWITCH,
	SCOPE_FOR,
	SCOPE_WHILE,
	SCOPE_EMPTY,
} SCOPE_TYPE;

typedef struct CompilerToken{
	constant value = 0;
	std::string token;
	TOKEN tokKind = TOKEN::UNKNOWN;
	uint32_t line = 0;
};

/**
*    Compiler tree vvv
*            
*										Root: Subroutine container
*													|
*						  ___________________________________________________
*						  |					|				|				|
*						Sub0               Sub1            ...			   SubN
*						  |
*             _________________________
*			  |           |            |
*			Scope1      Scope2      ScopeN
*			 |			  |            |
*        ___________
*	    |			|
*	 Scope1_1    Scope1_2...
* 
*/
struct Subroutine;


struct ScopeInfo {
	SCOPE_TYPE kind = SCOPE_TYPE::SCOPE_INVALID;
	std::string name = "";
	std::vector<CompilerToken> tokens;
};

struct Scope {
	ScopeInfo scopetype;
	size_t size = 0;
	std::vector <CompilerToken> codeblock;
	std::vector <constant> parsedblock;

	//Next leaves
	std::vector <Scope> in_scope;
	Subroutine* pParent;
};


struct Subroutine {
	size_t argc = 0;
	uint32_t address;
	std::string name;
	std::map<std::string, uint32_t> LabelMap;
	Scope sub_body;
};

class ScriptCompiler {
public:
	void Initialize(std::string, std::string);
	std::map<std::string, uint32_t> calcAddress();
	bool tokenizeSrc();
	//int parseCommand(std::string&, constant&, int, int);
	void parseSrc();
private:
	//1st phase
	CompilerToken tokenizeItem(std::string&, size_t);

	//2nd phase
	void __adjustItems();
	void __getScopes();
	void __calculateFunctionOffsets();


	std::string m_input;
	std::string m_output;

	std::string m_Text;

	size_t m_Size;
	size_t m_Pos = 0;
	std::vector<char*> m_OutFile;


	std::vector<CompilerToken> m_TokenVec;
	std::vector<std::string> m_StringVec;
	std::map<std::string, uint32_t> m_StringVecEx;

	std::map<std::string, Subroutine> m_SubroutineTree;
	std::map<std::string, uint32_t> m_SubroutineAddress;

	FILE* m_pOutputfile;
};