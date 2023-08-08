#include "FileReader.h"
#include <regex>
#include <fstream>


typedef enum {
	FAIL = -1,
	END,
	STRING,
	INTEGER,
	REAL,
	COMMA = 64,
	SPACE,
	FUNCTION,

} ARG_TYPE;

void ScriptCompiler::Initialize(std::string input, std::string output) {
	TextFile src;

	if (src.loadFile(input)) {
		m_input = input;
		m_output = output;

		this->m_Text = src.m_DataStr;
		if (this->tokenizeSrc()) {
			this->parseSrc();
		}
	}
	//calcAddress();
}


std::map<std::string, uint32_t> ScriptCompiler::calcAddress() {
	std::map<std::string, uint32_t> ret;
	size_t pos = 0;
	size_t subPos = 0;
	size_t linepos = 0;
	const size_t size = m_Size;
	std::string line = "";
	std::string subName = "";
	//char* buffer = m_pBuffer;
	char curChar;
	bool onSub = false;

	while (pos < size) {
	}
	return ret;
}


/**
* identifiers:
* things like functions and labels for now.
* Start with a letter, not with a number.
* keywords:
*  Void, sub, switch, if, else, return (Though, is in the VM too), while, do
* Integers, Reals and String Literals.
* Other tokens:
* -Math
*	+ - * / % =  += -= *= /= %= ++ -- << >> >>= <<= & | ! != == > < >= <=  && ||
*
* -Separators
, : ; ( ) { }
*/

//1st phase: Split everything into tokens
bool ScriptCompiler::tokenizeSrc() {


	std::regex comand("^((([\\d|\\w]*)([\\\t|\\s]*)\\\(([^;]*)\\\)([\\\t|\\\s]*)\;([\\s|\\\t]*))?)");
	std::regex parameters("^(([\\\t|\\s]*)\\\(([^]*)\\\)([\\\t|\\\s]*)\;)?");
	std::regex textline("^(([^]*)([\\\n]{1}))");
	std::smatch match;
	
	//std::replace_if(src.begin(), src.end(), [&](char entry) { return entry == '\r' || entry == '\n'; }, ' ');
	std::string params;

	bool onSubroutine = false;
	uint32_t line = 1;
	//uint32_t line_max = std::count_if(this->m_Text.begin(), this->m_Text.end(), [&](char c) {return c == '\n'; });
	//std::cout << "\n";
	std::string src = this->m_Text;
	std::ifstream input;
	input.open(this->m_input, std::ios::in);
	bool cont = true;
	try {
		CompilerToken get;
		while (getline(input, src) && cont) {
			do {
				get = this->tokenizeItem(src, line);
				if (cont = get.tokKind != TOKEN::UNKNOWN) {
					if (get.tokKind != TOKEN::IGNORE)
						this->m_TokenVec.push_back(get);
				}
				else
					throw get;
			} while (cont && src.size());
			line++;
		}
	}
	catch (CompilerToken except) {
		std::cout << "Unrecognized token: '" << except.token << "' in line " << except.line << ".\n";
		return false;
	}
	return true;
}

//1st phase subprocess: Get a token from a string, also asign a line
CompilerToken ScriptCompiler::tokenizeItem(std::string& params, size_t line) {
	static std::regex real_ex("^(([\\d]+)\\.([\\d]*))");
	static std::regex default_reg("^([^])");
	//static std::regex new_line("^(([\\\n])([^\\\n]))"); //Space and tab regex
	static std::regex tokenKind[] = {
		std::regex("^(([a-z|A-Z]+)([\\w]*))"), //Identifier and keyword regex.
		std::regex("^(([a-z|A-Z]+)([\\w]*))"), //Identifier and keyword regex.
		std::regex("^([\\d]+)\\.([\\d]*)f?"), //Real regex.
		std::regex("^([\\d]+)"), //Integer regex.
		std::regex("^(\\\"([^\\\"]*)\\\")"), //String regex.
		std::regex("^([(\\\{)|(\\\})|(\\\;)|(\\\:)|(\\\()|(\\\))|(\\,)|(\\@)|(#)])"), //separator regex
		std::regex("^([\\\+|\\\*|/|\\\-|%|(\\\+\\\+)|(\\\-\\\-)|(\\\+=)|(\\\*=)|(/=)|(\\\-=)|(%=)|(<<)|(>>)|(<<=)|(>>=)|&|\\\||(&&)|(\\\|\\\|)])"), //operator regex
		std::regex("^(#([a-z|A-Z]+)([\\w|\\d]*))"), //preprocessor regex
		//std::regex("^($)"), //end of script regex
		std::regex("^([\\s|\\\t|\\\r]+)"), //Space and tab regex
	};
	std::smatch match, match_f;
	std::string strval;

	if(std::regex_search(params, match, tokenKind[TOKEN::WHITESPACE - 1]))
		params = match.suffix();


	CompilerToken tok;
	tok.tokKind = TOKEN::UNKNOWN;
	tok.line = line;
	if (!params.size())
		tok.tokKind = TOKEN::IGNORE;
	else {
		for (int i = 0; i < TOKEN::EOSCRIPT; i++) {

			if (std::regex_search(params, match, tokenKind[i])) {
				//for (auto str: match) {
				//	std::cout << str << "\n";
				//}
				switch (i) {
				case TOKEN::REAL_LIT:
					if (std::regex_search(params, match_f, real_ex)) { //Real regex.
						tok.token = match_f[0];
					}
					break;
				case TOKEN::KEYWORD:
				case TOKEN::IDENTIFIER:
				case TOKEN::INT_LIT:
				case TOKEN::SEPARATOR:
				case TOKEN::ARITMETHIC:
				case TOKEN::PREPROCESSOR:
					tok.token = match[0];
					break;
				case TOKEN::STRING_LIT:
					tok.token = match[2];
					break;
				}
				params = match.suffix();
				tok.tokKind = (TOKEN)(TOKEN::KEYWORD + i);
				std::cout << tok.token << '\n';
				return tok;
			}
		}
	}
	std::regex_search(params, match, default_reg);
	tok.token = match[0];
	return tok;
}

/*

//Return argument count...
int ScriptCompiler::parseCommand(std::string& params, constant& reg, int past, int argc) {

	//int argc = 0;

	std::regex comma("^([\\s|\\\t]*),([\\s|\\\t]*)");
	std::regex space("^([\\s|\\\t]+)");
	std::regex str("^(\\\"([^]*)\\\")");
	std::regex end("^($)");
	std::regex num("^(([\\d]+))");
	std::regex real("^(([\\d]+)\\.([\\d]*)f?)");
	std::regex function("^(([\\w]+)([\\w|\\d]*)([\\\t|\\s]*)\\\(([^]*)\\\))^([\\s|\\\t]*)");
	std::regex label("^(([\\w]+)([\\w|\\d]*))");
	std::smatch match;
	int kind =
		(std::regex_search(params, match, str)) ? ARG_TYPE::STRING :
		(std::regex_search(params, match, real)) ? ARG_TYPE::REAL :
		(std::regex_search(params, match, num)) ? ARG_TYPE::INTEGER :
		(std::regex_search(params, match, comma)) ? ARG_TYPE::COMMA :
		(std::regex_search(params, match, space)) ? ARG_TYPE::SPACE :
		(std::regex_search(params, match, function)) ? ARG_TYPE::FUNCTION :
		(std::regex_search(params, match, end)) ? 0 :
		0;

		if((kind == ARG_TYPE::STRING || kind == ARG_TYPE::INTEGER || kind == ARG_TYPE::REAL) && argc >= 1)
			if (!(past == ARG_TYPE::END || past == ARG_TYPE::COMMA)) {
				std::cout << "\nException: found another argument without separation, stopping...\n";
				return ARG_TYPE::FAIL;
			}
		switch (kind) {
			case ARG_TYPE::STRING: {
				bool skip = false;

				//for (auto get : match) {
				std::cout << match[2] << "\n";
				//}

				std::string temp = match[2];
				if (this->m_StringVecEx.find(temp) == this->m_StringVecEx.end()) {
					this->m_StringVecEx.insert({temp, this->m_StringVecEx.size()});
				}

				reg = (int)this->m_StringVecEx[temp];

				for (auto it = m_StringVec.begin(); it != m_StringVec.end(); ++it) {
					if (skip = *it == temp) {
						break;
					}
				}
				if(!skip)
					this->m_StringVec.push_back(temp);
				params = match.suffix();
			}
				  //argc++;
			  break;

			case ARG_TYPE::INTEGER: {
				//for (auto get : match) {
					std::cout << match[0] << "\n";
				//}
					reg = (int)std::stoi(match[0]);
				params = match.suffix();
			}
				  //argc++;
			  break;
			case ARG_TYPE::REAL: {
				//for (auto get : match) {
				std::cout << match[0] << "\n";
				//}
				reg = (float)std::stof(match[0]);
				params = match.suffix();
			}
								  //argc++;
								  break;
			case ARG_TYPE::COMMA: {
				//for (auto get : match) {
				//	std::cout << get << "\n";
				//}
				params = match.suffix();
			}
			break;
			case ARG_TYPE::SPACE: {
				for (auto get : match) {
					//std::cout << get << "\n";
				}
				params = match.suffix();
			}
								break;
			case ARG_TYPE::FUNCTION: {
				for (auto get : match) {
					std::cout << get << "\n";
				}
				std::cout << "\nMatch: " << match[0] << "\n";
				params = match.suffix();
			}
			break;
		}
	//std::cout <<  "\n Argument cnt: " << argc << "\n";
	return kind;
}
*/
//Second phase
void ScriptCompiler::parseSrc() {
	this->__adjustItems();
	this->__getScopes();
	this->__calculateFunctionOffsets();
}

//Second phase part 1: Get all the values and confirm and correct keywords, also put Strings in a vector and return an ID
void ScriptCompiler::__adjustItems() {
	static std::regex keywordRegex[]{
		std::regex("function$"),
		std::regex("if$"),
		std::regex("else$"),
		std::regex("switch$"),
		std::regex("for$"),
		std::regex("while$"),
		std::regex("break$"),
		std::regex("return$"),
	};
	bool onSubRoutine = false;
	std::smatch match;
	int i = 0, it = 0;

	const size_t size = this->m_TokenVec.size();
	for (it = 0; it <  size; it++) {
		CompilerToken temp = this->m_TokenVec[it];
		switch (temp.tokKind) {
			case TOKEN::INT_LIT: {
				temp.value = std::stoi(temp.token);
			} break;
			case TOKEN::REAL_LIT: {
				temp.value = std::stof(temp.token);
			} break;

			case TOKEN::STRING_LIT: {
				if (this->m_StringVecEx.find(temp.token) == this->m_StringVecEx.end()) {
					this->m_StringVecEx.insert({ temp.token, this->m_StringVecEx.size() });
				}
				temp.value = (int)this->m_StringVecEx[temp.token];
			} break;

			case TOKEN::KEYWORD: {
				bool isKeyword = false;
				for (int v = 0; v <= COMPKEYWORD::RETURN; v++) {
					if (isKeyword = std::regex_search(temp.token, match, keywordRegex[v])){
						temp.value = v;
						temp.token = match[0];
						break;
					}
				}
				if (!isKeyword) {
					temp.tokKind = TOKEN::IDENTIFIER;
				}
			} break;
		};
		this->m_TokenVec[it] = temp;
	}
}

//Second phase part 2: Get the scopes and retrieve the contents
void ScriptCompiler::__getScopes() {
	bool onSubroutine = false;
	Subroutine tempSub;
	Scope temp;
	const size_t size = this->m_TokenVec.size();
	size_t index = 0;
	std::stack<Scope> scopeStack;
	std::stack<size_t> start_pos;
	COMPKEYWORD lastKeyword = COMPKEYWORD::KEY_INVALID;
	ScopeInfo scinfo;
	for (int i = 0; i < size; i++) {
		CompilerToken ttok = this->m_TokenVec[i];
		const std::string comp = ttok.token;
		switch (ttok.tokKind) {

		case TOKEN::SEPARATOR: {
			if ((comp == "{" || comp == "}")) {
				if (comp == "{") {
					scopeStack.push(temp);
					if (lastKeyword == COMPKEYWORD::KEY_INVALID) {
						std::cout << "Non-keyword-owned scope not allowed.\n";
					}
					else if (lastKeyword == COMPKEYWORD::ELSE) {
					}
					else {
						scopeStack.top().scopetype = scinfo;
						scinfo = ScopeInfo();
					}
					index++;
					start_pos.push(i + 1);
				}
				else if (comp == "}") {
					if (index-- > 0) {
						const size_t start = start_pos.top();
						const size_t it = i - start;
						start_pos.pop();
						for (int j = 0; j < it; j++) {
							scopeStack.top().codeblock.push_back(this->m_TokenVec[start + j]);
						}
						//scopeStack.top().scopetype = scinfo;
						lastKeyword = COMPKEYWORD::KEY_INVALID;
					}
					else {
						std::cout << "Error: Unstarted scope detected.\n";
					}
					if (onSubroutine) {
						if (index == 0 && scopeStack.size() > 0) {
							tempSub.sub_body = scopeStack.top();
							scopeStack.pop();
							tempSub.name = tempSub.sub_body.scopetype.name;
							onSubroutine = false;
							this->m_SubroutineTree.insert({ tempSub.name, tempSub });

						}
						else {
							Scope tsc = scopeStack.top();
							scopeStack.pop();
							scopeStack.top().codeblock.push_back({ scopeStack.top().codeblock.size(), tsc.scopetype.name, TOKEN::SCOPE });
							scopeStack.top().in_scope.push_back(tsc);
						}
					}
				}
			}
			else if (comp == "(" || comp == ")") {
				if (comp == ")") {
					std::cout << "Error: Unpaired \")\".\n";
				}
				for (++i; this->m_TokenVec[i].token != ")"; i++) {
					if (i < size) {
						scinfo.tokens.push_back(this->m_TokenVec[i]);
					}
					else {
						std::cout << "Error: Missing \")\".\n";
					}
				}
			}
		}break;
		case TOKEN::KEYWORD: {
			lastKeyword = (COMPKEYWORD)(int)ttok.value;
			scinfo.kind = (SCOPE_TYPE)(lastKeyword + ((int)ttok.value >= 3));
			if ((ttok.tokKind == TOKEN::KEYWORD && lastKeyword == COMPKEYWORD::KEY_VOID) ||
				(ttok.tokKind == TOKEN::KEYWORD && lastKeyword == COMPKEYWORD::ELSE)) {
				switch (lastKeyword) {
				case COMPKEYWORD::KEY_VOID: {
					if (++i < size) {
						if (this->m_TokenVec[i].tokKind == TOKEN::IDENTIFIER) {
							scinfo.kind = SCOPE_TYPE::SCOPE_VOID;
							scinfo.name = this->m_TokenVec[i].token;
							onSubroutine = true;
							index = 0;
						}
					}
					else {
						std::cout << "Error: Expected identifier, but reached End of File.\n";
					}
				}break;
				case COMPKEYWORD::ELSE: {
					if (i + 1 < size && this->m_TokenVec[i].tokKind == TOKEN::KEYWORD && (int)this->m_TokenVec[i].value == COMPKEYWORD::IF) {
						scinfo.kind = SCOPE_TYPE::SCOPE_ELSE_IF;
						i++;
					}
				}break;
				}
			}
			/*{
				int j = i + 2;
				if (j < size) {
					CompilerToken test = this->m_TokenVec[j];
					if (test.tokKind == TOKEN::SEPARATOR && test.token == "(") {
						for (; j < size && test.token != ")"; j++) {

						}
					}
				}
			}*/
		} break;
		case TOKEN::IDENTIFIER: {
			if (++i > size && this->m_TokenVec[i].tokKind == TOKEN::SEPARATOR && this->m_TokenVec[i].token == ":") {
				if (onSubroutine) {
					tempSub.LabelMap.insert({ this->m_TokenVec[i - 1].token, 0});
				}
				else {
					std::cout << "Error: Can't create labels outside a Subroutine.\n";
				}
			}
			else {
				std::cout << "Error: Found unasigned identifier.\n";
			}
		}break;
		}
	}
	if (index > 0) {
		std::cout << "Unclosed scope detected.\n";
	}
}
//Second phase part 3: Calculate Function offsets
void ScriptCompiler::__calculateFunctionOffsets() {
	bool onSubroutine = false;
	size_t offset = 0;
	const size_t size = this->m_TokenVec.size();
	Subroutine tempSub;
	Scope temp;
	std::stack<Scope> currentScope;
	size_t index = 0;

	for (int i = 0; i < size;) {
		CompilerToken temp = this->m_TokenVec[i];
		switch (temp.tokKind) {
			case TOKEN::KEYWORD: {/*
				switch((int)temp.value){
					case COMPKEYWORD::VOID: {
						CompilerToken test;
						if (i + 2 < size) {
							if (this->m_TokenVec[i + 1].tokKind == TOKEN::IDENTIFIER 
								&& this->m_TokenVec[i + 2].tokKind == TOKEN::SEPARATOR 
								&& this->m_TokenVec[i + 2].token == "(") {
								test = this->m_TokenVec[i + 1];
								if (onSubroutine) {
									std::cout << "Can't create Subroutine inside a Subroutine.\n";
								}
								else {
									if (this->m_Address.find(this->m_TokenVec[i + 1].token) == this->m_Address.end()) {
										this->m_Address.insert({ this->m_TokenVec[i + 1].token, offset });
										onSubroutine = true;
										i += 3;
									}
									else {
										std::cout << "Redefinition of Subroutine " << this->m_TokenVec[i + 1].token << " not allowed.\n";
									}
								}
							}
						}
					}break;

					case COMPKEYWORD::IF: {
						if (onSubroutine) {
							CompilerToken test;
							if (i + 1 < size) {
								test = this->m_TokenVec[i + 1];
								if (test.tokKind == TOKEN::SEPARATOR && test.token == "(") {

								}
							}
						}
					} break;
				}*/
			}break;
			case TOKEN::SEPARATOR: {
				//if
			} break;
		}
	}
}