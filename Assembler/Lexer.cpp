#include "Lexer.h"
#include <iostream>
#include <fstream>
#include <regex>

Lexer::Lexer() {
	this->m_CommandMap =
	{
		//Stack operations
		{"pushc", ScriptCommand::PUSHC},
		{"pushr", ScriptCommand::PUSHR},
		{"pushv", ScriptCommand::PUSHV},
		{"pushs", ScriptCommand::PUSHS},
		{"pushe", ScriptCommand::PUSHE},
		{"pushcn", ScriptCommand::PUSHCN},

		{"popr", ScriptCommand::POPR},
		{"popv", ScriptCommand::POPV},
		{"pops", ScriptCommand::POPS},
		{"pope", ScriptCommand::POPE},
		{"popcn", ScriptCommand::POPCN},

		{"movc", ScriptCommand::MOVC},
		{"movcs", ScriptCommand::MOVCS},
		{"movv", ScriptCommand::MOVV},
		{"movce", ScriptCommand::MOVCE},
		{"movccn", ScriptCommand::MOVCCN},

		//Math
		{"addi", ScriptCommand::ADDI},
		{"addf", ScriptCommand::ADDF},
		{"subi", ScriptCommand::SUBI},
		{"subf", ScriptCommand::SUBF},
		{"muli", ScriptCommand::MULI},
		{"mulf", ScriptCommand::MULF},
		{"divi", ScriptCommand::DIVI},
		{"divf", ScriptCommand::DIVF},
		{"maxi", ScriptCommand::MAXI},
		{"maxf", ScriptCommand::MAXF},
		{"mini", ScriptCommand::MINI},
		{"minf", ScriptCommand::MINF},
		{"mod", ScriptCommand::MOD},

		{"sin", ScriptCommand::SIN},
		{"cos", ScriptCommand::COS},
		{"atan2i", ScriptCommand::ATAN2I},
		{"atan2f", ScriptCommand::ATAN2F},
		{"sqrti", ScriptCommand::SQRTI},
		{"sqrtf", ScriptCommand::SQRTF},
		{"hypoti", ScriptCommand::HYPOTI},
		{"hypotf", ScriptCommand::HYPOTF},
		{"normrad", ScriptCommand::NORMRAD},

		{"negi", ScriptCommand::NEGI},
		{"negf", ScriptCommand::NEGF},
		{"itof", ScriptCommand::ITOF},
		{"ftoi", ScriptCommand::FTOI},
		{"shl", ScriptCommand::SHL},
		{"shr", ScriptCommand::SHR},

		{"eqi", ScriptCommand::EQI},
		{"eqf", ScriptCommand::EQF},
		{"neqi", ScriptCommand::NEQI},
		{"neqf", ScriptCommand::NEQF},
		{"greati", ScriptCommand::GREATI},
		{"greatf", ScriptCommand::GREATF},
		{"geqi", ScriptCommand::GEQI},
		{"geqf", ScriptCommand::GEQF},
		{"lessi", ScriptCommand::LESSI},
		{"lessf", ScriptCommand::LESSF},
		{"leqi", ScriptCommand::LEQI},
		{"leqf", ScriptCommand::LEQF},

		{"and", ScriptCommand::AND},
		{"or", ScriptCommand::OR},
		{"xor", ScriptCommand::XOR},
		{"not", ScriptCommand::NOT},

		{"crand", ScriptCommand::CRAND},
		{"randi", ScriptCommand::RANDI},
		{"randf", ScriptCommand::RANDF},
		{"randang", ScriptCommand::RANDANG},

		{"absi", ScriptCommand::ABSI},
		{"absf", ScriptCommand::ABSF},

		//System
		{"nop", ScriptCommand::NOP},
		{"jmp", ScriptCommand::JMP},
		{"tjmp", ScriptCommand::TJMP},
		{"fjmp", ScriptCommand::FJMP},
		{"call", ScriptCommand::CALL},
		{"tcall", ScriptCommand::TCALL},
		{"fcall", ScriptCommand::FCALL},
		{"ret", ScriptCommand::RET},
		{"int", ScriptCommand::SETINTERRUPT},
		{"retc", ScriptCommand::RETCODE},
		{"objcount", ScriptCommand::OBJCOUNT},

		{"stkdel", ScriptCommand::STACKDELETE},
		{"varalloc", ScriptCommand::VARALLOC},
		{"vardealloc", ScriptCommand::VARDEALLOC},
		{"objcreate", ScriptCommand::OBJCREATE},
		{"objclosest", ScriptCommand::OBJCLOSEST},
		{"objexist", ScriptCommand::OBJEXIST},
		{"delint", ScriptCommand::DELINTERRUPT},
		{"killobj", ScriptCommand::KILLOBJ},
		{"intret", ScriptCommand::INTRET},
		{"ownptr", ScriptCommand::OWNPTR},
		{"createthread", ScriptCommand::CREATETHREAD},
		{"stopthread", ScriptCommand::STOPTHREAD},
		{"resumethread", ScriptCommand::RESUMETHREAD},
		{"chgtask", ScriptCommand::CHANGETHREADTASK},
		{"killthread", ScriptCommand::KILLTHREAD},

		{ "pushadd", ScriptCommand::PUSHADD },
		{ "popcall", ScriptCommand::POPCALL },

		//Audio
		{"playmusic", ScriptCommand::PLAYMUSIC},
		{"pausemusic", ScriptCommand::PAUSEMUSIC},
		{"resumemusic", ScriptCommand::RESUMEMUSIC},
		{"stopmusic", ScriptCommand::STOPMUSIC},
		{"addvolume", ScriptCommand::ADDVOLUME},
		{"setvolume", ScriptCommand::SETVOLUME},
		{"playse", ScriptCommand::PLAYSOUND},
		{"musoffset", ScriptCommand::SETPLAYOFFSET},
		{"playsel", ScriptCommand::PLAYSOUNDLOOP},
		{"stopsel", ScriptCommand::STOPSOUNDLOOP},
		{"playseo", ScriptCommand::PLAYSOUNDOFFSET},
		{"isplayingse", ScriptCommand::LASTSOUNDPLAYING},
		{"isplayingall", ScriptCommand::ALLSOUNDPLAYING},

		//String
		{"pushstr", ScriptCommand::PUSHSTR},
		{"pushstrs", ScriptCommand::PUSHSTRS},
		{"popstr", ScriptCommand::POPSTR},
		{"topstr", ScriptCommand::TOPSTR},
		{"i2str", ScriptCommand::I2STR},
		{"f2str", ScriptCommand::F2STR},
		{"concatstr", ScriptCommand::CONCATSTR},
		{"strsize", ScriptCommand::STRSIZE},
		{"newl", ScriptCommand::NEWL},
		{"drawstr", ScriptCommand::DRAWSTR},
		{"drawstrn", ScriptCommand::DRAWSTRN},
		{"clearstr", ScriptCommand::CLEARSTR},

		//Debug
		{"printf", ScriptCommand::PRINTF},
		{"devbreak", ScriptCommand::BREAKPOINT},
	};

	this->m_PrepropMap = {
		{"model", ASMPREPROCESOR::ASM_MODEL3D},
		{"quad", ASMPREPROCESOR::ASM_QUAD},
		{"shader2d", ASMPREPROCESOR::ASM_SHADER2D},
		{"shader3d", ASMPREPROCESOR::ASM_SHADER3D},
		{"music", ASMPREPROCESOR::ASM_MUSIC},
		{"sound", ASMPREPROCESOR::ASM_SOUND},
		{"string", ASMPREPROCESOR::ASM_TEXT},
		{"const", ASMPREPROCESOR::ASM_CONSTANT},
		{"import", ASMPREPROCESOR::ASM_IMPORT},
	};
}

void Lexer::Initialize(std::string input) {

	TextFile src;

	this->m_Input = input;
	//if (src.loadFile(input)) {
	//	this->m_Input = input;
	//	//this->m_Src = src.m_DataStr;
	//}
}

bool Lexer::tokenizeSrc() {
	std::string params;

	bool onSubroutine = false;
	uint32_t line = 1;
	//uint32_t line_max = std::count_if(this->m_Text.begin(), this->m_Text.end(), [&](char c) {return c == '\n'; });
	//std::cout << "\n";
	std::string src;
	std::ifstream input;
	input.open(this->m_Input, std::ios::in);
	if (input.is_open()) {
		bool cont = true;
		try {
			AsmToken get;
			while (getline(input, src) && cont) {
				do {
					get = this->tokenizeItem(src, line);
					if (cont = get.tok != ASMTOKEN::UNKNOWN2) {
						if (get.tok != ASMTOKEN::IGNORE2)
							this->m_TokenArr.push_back(get);
					}
					else
						throw get;
				} while (cont && src.size());
				line++;
			}
		}
		catch (AsmToken except) {
			std::cerr << "Lexer::tokenizeSrc() > Unrecognized token: '" << except.val << "' in line " << except.line << ".\n";
			return false;
		}
		return true;
	}
	else {
		std::cerr << "Lexer::tokenizeSrc() > Failed to open \"" << this->m_Input << "\".\n";
		return false;
	}
}

AsmToken Lexer::tokenizeItem(std::string& params, size_t line) {
	static std::regex real_ex("^([-]?([\\d]+)\\.([\\d]*))");
	static std::regex default_reg("^([^])");
	//static std::regex new_line("^(([\\\n])([^\\\n]))"); //Space and tab regex
	static std::regex tokenKind[] = {
		std::regex("^(([a-z|A-Z]+)([\\w]*))"), //Identifier and keyword regex.
		std::regex("^(([a-z|A-Z]+)([\\w]*))"), //Identifier and keyword regex.
		std::regex("^([-]?[\\d]+)\\.([\\d]*)f?"), //Real regex.
		std::regex("^([-]?[\\d]+)"), //Integer regex.
		std::regex("^(\\\"([^\\\"]*)\\\")"), //String regex.
		std::regex("^([(\\\{)|(\\\})|(\\\;)|(\\\:)|(\\\()|(\\\))|(\\,)|(\\@)|(#)])"), //separator regex
		std::regex("^([\\.]([\\w]+))"), //address regex
		std::regex("^(#([a-z|A-Z]+)([\\w|\\d]*))"), //preprocessor regex
		//std::regex("^($)"), //end of script regex
		std::regex("^([\\s|\\\t|\\\r]+)"), //Space and tab regex
	};
	std::smatch match, match_f;
	std::string strval;

	if (std::regex_search(params, match, tokenKind[ASMTOKEN::WHITESPACE2 - 1]))
		params = match.suffix();


	AsmToken tok;
	tok.tok = ASMTOKEN::UNKNOWN2;
	tok.line = line;
	if (!params.size())
		tok.tok = ASMTOKEN::IGNORE2;
	else {
		for (int i = 0; i < ASMTOKEN::EOSCRIPT2; i++) {

			if (std::regex_search(params, match, tokenKind[i])) {
				//for (auto str: match) {
				//	std::cout << str << "\n";
				//}
				switch (i) {
				case ASMTOKEN::REAL_LIT2:
					if (std::regex_search(params, match_f, real_ex)) { //Real regex.
						tok.val = match_f[0];
					}
					break;
				case ASMTOKEN::ADDRESS:
					tok.val = match[2];
					break;
				case ASMTOKEN::KEYWORD2:
				case ASMTOKEN::IDENTIFIER2:
				case ASMTOKEN::INT_LIT2:
				case ASMTOKEN::SEPARATOR2:
				case ASMTOKEN::PREPROCESSOR2:
					
					tok.val = match[0];
					break;
				case ASMTOKEN::STRING_LIT2:
					tok.val = match[2];
					break;
				}
				params = match.suffix();
				tok.tok = (ASMTOKEN)(ASMTOKEN::KEYWORD2 + i);
#ifdef _DEBUG
				std::cout << tok.val << '\n';
#endif
				return tok;
			}
		}
	}
	std::regex_search(params, match, default_reg);
	tok.val = match[0];
	return tok;
}

std::vector<AsmToken> Lexer::getTokens() {
	return this->m_TokenArr;
}

void Lexer::initValues() {
	bool onSubRoutine = false;
	std::smatch match;
	bool preprop = false;

	const size_t size = this->m_TokenArr.size();
	for (int it = 0; it < size; it++) {
		AsmToken temp = this->m_TokenArr[it];
		switch (temp.tok) {
			case ASMTOKEN::KEYWORD2: {
				if (this->m_CommandMap.find(temp.val) != this->m_CommandMap.end()) {
					temp.numval = (int)this->m_CommandMap[temp.val];
				}
				else if (this->m_PrepropMap.find(temp.val) != this->m_PrepropMap.end()) {
					temp.numval = (int)this->m_PrepropMap[temp.val];
					temp.tok = ASMTOKEN::PREPROCESSOR2;
				}
				else if (temp.val == "rad") {
					temp.tok = ASMTOKEN::RAD2;
				}
				else {
					temp.tok = ASMTOKEN::IDENTIFIER2;
				}
			} break;
			case ASMTOKEN::REAL_LIT2: {
				temp.numval = (float)std::stod(temp.val);
			} break;
			case ASMTOKEN::INT_LIT2: {
				temp.numval = (uint32_t)std::stoll(temp.val);
			} break;
		};
		this->m_TokenArr[it] = temp;
	}
}