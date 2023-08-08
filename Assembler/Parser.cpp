#include "Parser.h"

Parser::Parser() {
	this->m_offsetEx = 0;
}

void Parser::Initialize(const std::string& in) {
	this->m_InName = in;
	this->m_Lexer.Initialize(in);
}

bool Parser::parseTokens() {
	size_t pos = 0;
	bool onError = false;
	if (this->m_Lexer.tokenizeSrc()) {
		this->m_Lexer.initValues();
		this->m_TokArr = this->m_Lexer.getTokens();
		if (this->calcAddresses()) {
			const size_t size = this->m_TokArr.size();
			try {
				for (size_t i = 0; i < size && !onError; ) {
					switch (this->m_TokArr[i].tok) {
					case ASMTOKEN::ADDRESS:
						i++;
						break;
					case ASMTOKEN::KEYWORD2: {
						i++;
						pos++;
						this->m_NumArray.push_back(this->m_TokArr[i - 1].numval);
						if (this->m_TokArr[i - 1].tok == ASMTOKEN::KEYWORD2) {
							uint32_t cmd = this->m_TokArr[i - 1].numval;
							int addoff;
							switch (cmd) {
							case ScriptCommand::JMP:
							case ScriptCommand::TJMP:
							case ScriptCommand::FJMP:
							case ScriptCommand::SETINTERRUPT:
							case ScriptCommand::OBJCREATE:
							case ScriptCommand::CALL:
							case ScriptCommand::TCALL:
							case ScriptCommand::FCALL:
							case ScriptCommand::CREATETHREAD:
							case ScriptCommand::CHANGETHREADTASK:
							case ScriptCommand::PUSHADD:
								addoff = 2;
								if (this->m_AddressMap.find(this->m_TokArr[i].val) != this->m_AddressMap.end()) {
									addoff = ((int)this->m_AddressMap[this->m_TokArr[i].val] - (pos - 1));
									this->m_NumArray.push_back((int)addoff);
									i++;
									pos++;
									if (cmd == ScriptCommand::OBJCREATE) {
										if (this->m_TokArr[i].tok == ASMTOKEN::INT_LIT2) {
											this->m_NumArray.push_back(this->m_TokArr[i].numval);
											i++;
											pos++;
										}
										else {
											throw this->m_TokArr[i];
											onError = true;
										}
									}
								}
								else {
									throw this->m_TokArr[i];
									onError = true;
								}
								break;
							case ScriptCommand::MOVC:
							case ScriptCommand::MOVCS:
							case ScriptCommand::MOVV:
							case ScriptCommand::MOVCE:
							case ScriptCommand::MOVCCN:
								if (this->m_TokArr[i].tok == ASMTOKEN::IDENTIFIER2) {
									if (this->m_DefinedTokenMap.find(this->m_TokArr[i].val) != this->m_DefinedTokenMap.end())
										this->m_NumArray.push_back(this->m_DefinedTokenMap[this->m_TokArr[i].val].numval);
									else {
										throw this->m_TokArr[i];
										onError = true;
										break;
									}
								}
								else if (this->m_TokArr[i].tok == ASMTOKEN::INT_LIT2) {
									this->m_NumArray.push_back(this->m_TokArr[i].numval);
								}
								else {
									throw this->m_TokArr[i];
									onError = true;
									break;
								}
								pos++;
								i++;
							case ScriptCommand::PUSHC:
							case ScriptCommand::PUSHR:
							case ScriptCommand::PUSHV:
							case ScriptCommand::PUSHS:
							case ScriptCommand::PUSHE:
							case ScriptCommand::PUSHCN:
							case ScriptCommand::POPR:
							case ScriptCommand::POPV:
							case ScriptCommand::POPS:
							case ScriptCommand::POPE:
							case ScriptCommand::POPCN:
							case ScriptCommand::RETCODE:
							case ScriptCommand::STACKDELETE:
							case ScriptCommand::VARALLOC:
							case ScriptCommand::PUSHSTR:
								switch (this->m_TokArr[i].tok) {
								case ASMTOKEN::IDENTIFIER2:
									if (this->m_DefinedTokenMap.find(this->m_TokArr[i].val) != this->m_DefinedTokenMap.end())
										this->m_NumArray.push_back(this->m_DefinedTokenMap[this->m_TokArr[i].val].numval);
									else {
										throw this->m_TokArr[i];
										onError = true;
									}
									break;
								case ASMTOKEN::RAD2:
									if (i + 1 < size)
										i++;
									else
										break;
									switch (this->m_TokArr[i].tok) {
									case ASMTOKEN::INT_LIT2:
										this->m_TokArr[i].numval = (float)(int)this->m_TokArr[i].numval;
									case ASMTOKEN::REAL_LIT2:
										this->m_TokArr[i].numval = (PI * (double)(float)this->m_TokArr[i].numval) / 180.0;
									}
								case ASMTOKEN::INT_LIT2:
								case ASMTOKEN::REAL_LIT2:
									this->m_NumArray.push_back(this->m_TokArr[i].numval);
									break;
								default:
									throw this->m_TokArr[i];
									onError = true;
									break;
								}
								i++;
								pos++;
								break;
							}
						}
					}break;
					case ASMTOKEN::PREPROCESSOR2: {
						switch ((int)this->m_TokArr[i].numval) {
						case ASMPREPROCESOR::ASM_CONSTANT:
							//Something like #define, done!
							i++;
							if (i + 2 < size &&
								this->m_TokArr[i].tok == ASMTOKEN::IDENTIFIER2 &&
								(this->m_TokArr[i + 1].tok == ASMTOKEN::INT_LIT2 ||
									this->m_TokArr[i + 1].tok == ASMTOKEN::REAL_LIT2)) {
								if (this->m_DefinedTokenMap.find(this->m_TokArr[i].val) == this->m_DefinedTokenMap.end())
									this->m_DefinedTokenMap.insert({ this->m_TokArr[i].val, this->m_TokArr[i + 1] });
								else
									this->m_DefinedTokenMap[this->m_TokArr[i].val] = this->m_TokArr[i + 1];
								i += 2;
								this->m_offsetEx += 2;
							}
							else {
								throw this->m_TokArr[i].numval;
							}
							break;
						case ASMPREPROCESOR::ASM_MUSIC: {
							i++;
							while (i + 4 < size &&
								this->m_TokArr[i].tok == ASMTOKEN::STRING_LIT2 &&
								this->m_TokArr[i + 1].tok == ASMTOKEN::INT_LIT2 &&
								this->m_TokArr[i + 2].tok == ASMTOKEN::INT_LIT2) {
								Music temp = { this->m_TokArr[i].val, (int)this->m_TokArr[i + 1].numval, (int)this->m_TokArr[i + 2].numval, 0 };
								this->m_FileHeader.musc.musicArr.push_back(temp);
								if (this->m_TokArr[i + 3].val == ",") {
									i += 4;
									this->m_offsetEx += 3;
									continue;
								}
								else if (this->m_TokArr[i + 3].val == ";") {
									i += 4;
									this->m_offsetEx += 3;
									break;
								}
								else {
									throw this->m_TokArr[i].numval;
								}
							}
						}break;
						case ASMPREPROCESOR::ASM_SHADER2D:
						case ASMPREPROCESOR::ASM_SHADER3D: {
							int kind = (int)this->m_TokArr[i].numval;
							i++;
							if (i + 3 < size &&
								this->m_TokArr[i].tok == ASMTOKEN::STRING_LIT2 &&
								this->m_TokArr[i + 1].tok == ASMTOKEN::STRING_LIT2 &&
								this->m_TokArr[i + 2].val == ";") {
								if (kind == ASMPREPROCESOR::ASM_SHADER2D) {
									this->m_FileHeader.quad.vshader = this->m_TokArr[i].val;
									this->m_FileHeader.quad.fshader = this->m_TokArr[i + 1].val;
									this->m_offsetEx += 2;
								}
								else if (kind == ASMPREPROCESOR::ASM_SHADER3D) {
									this->m_FileHeader.m3dl.vshader = this->m_TokArr[i].val;
									this->m_FileHeader.m3dl.fshader = this->m_TokArr[i + 1].val;
									this->m_offsetEx += 2;
								}
								else {
									throw this->m_TokArr[i].numval;
								}
								i += 3;
							}
						}break;
						case ASMPREPROCESOR::ASM_TEXT:
						case ASMPREPROCESOR::ASM_SOUND:
						case ASMPREPROCESOR::ASM_QUAD:
						case ASMPREPROCESOR::ASM_MODEL3D: {
							int kind = (int)this->m_TokArr[i].numval;
							i++;
							while (i + 1 < size) {
								if (this->m_TokArr[i].tok == ASMTOKEN::STRING_LIT2) {
									if (kind == ASMPREPROCESOR::ASM_QUAD)
										this->m_FileHeader.quad.quadArr.push_back(this->m_TokArr[i].val);
									else if (kind == ASMPREPROCESOR::ASM_MODEL3D)
										this->m_FileHeader.m3dl.modelArr.push_back(this->m_TokArr[i].val);
									else if (kind == ASMPREPROCESOR::ASM_SOUND)
										this->m_FileHeader.sndm.soundArr.push_back(this->m_TokArr[i].val);
									else if (kind == ASMPREPROCESOR::ASM_TEXT)
										this->m_FileHeader.StrContainer.push_back(this->m_TokArr[i].val);
									if (this->m_TokArr[i + 1].val == ",") {
										this->m_offsetEx++;
										i += 2;
									}
									else if (this->m_TokArr[i + 1].val == ";") {
										this->m_offsetEx++;
										i += 2;
										break;
									}
									else {
										throw this->m_TokArr[i].numval;
									}
								}
								else {
									throw this->m_TokArr[i];
								}
							}
							if (kind == ASMPREPROCESOR::ASM_QUAD)
								this->m_FileHeader.quad.size = this->m_FileHeader.quad.quadArr.size();
							else if (kind == ASMPREPROCESOR::ASM_MODEL3D)
								this->m_FileHeader.m3dl.size = this->m_FileHeader.m3dl.modelArr.size();
							else if (kind == ASMPREPROCESOR::ASM_SOUND)
								this->m_FileHeader.sndm.size = this->m_FileHeader.sndm.soundArr.size();
						}break;
						}
					}break;
					default:
						throw this->m_TokArr[i];
						onError = true;
						break;
					}
				}
			}
			catch (AsmToken tok) {
				std::cerr << "Parser::ParseTokens() > Error in line " << tok.line << ": " << tok.val << "\n";
			}
		}
	}
	else {
		std::cout << "Couldn't tokenize source.\n";
	}
}

bool Parser::calcAddresses() {
	const size_t size = this->m_TokArr.size();
	size_t pos = 0;
	bool onError = false;
	try {
		for (int i = 0; i < size && !onError; i++) {

			switch (this->m_TokArr[i].tok) {
			case ASMTOKEN::ADDRESS:
				if(this->m_AddressMap.find(this->m_TokArr[i].val) == this->m_AddressMap.end())
					this->m_AddressMap.insert({ this->m_TokArr[i].val, pos });
				else {
					onError = true;
					throw this->m_TokArr[i];
				}

				break;
			case ASMTOKEN::KEYWORD2: {
				pos++;
				uint32_t cmd = this->m_TokArr[i].numval;
				switch (cmd) {
					case ScriptCommand::JMP:
					case ScriptCommand::TJMP:
					case ScriptCommand::FJMP:
					case ScriptCommand::SETINTERRUPT:
					case ScriptCommand::OBJCREATE:
					case ScriptCommand::CALL:
					case ScriptCommand::TCALL:
					case ScriptCommand::FCALL:
					case ScriptCommand::CREATETHREAD:
					case ScriptCommand::CHANGETHREADTASK:
					case ScriptCommand::PUSHADD:
						i++;
						pos++;
						if (cmd != ScriptCommand::OBJCREATE)
							break;
						i++;
						pos++;
					break;
					case ScriptCommand::MOVC:
					case ScriptCommand::MOVCS:
					case ScriptCommand::MOVV:
					case ScriptCommand::MOVCE:
					case ScriptCommand::MOVCCN:
						i++;
						pos++;
					case ScriptCommand::PUSHC:
					case ScriptCommand::PUSHR:
					case ScriptCommand::PUSHV:
					case ScriptCommand::PUSHS:
					case ScriptCommand::PUSHE:
					case ScriptCommand::PUSHCN:
					case ScriptCommand::POPR:
					case ScriptCommand::POPV:
					case ScriptCommand::POPS:
					case ScriptCommand::POPE:
					case ScriptCommand::POPCN:
					case ScriptCommand::RETCODE:
					case ScriptCommand::STACKDELETE:
					case ScriptCommand::VARALLOC:
					case ScriptCommand::PUSHSTR:
						if (this->m_TokArr[i].tok == ASMTOKEN::RAD2)
							i++;
						i++;
						pos++;
					break;
				}
			}break;
			}
		}
	}
	catch (AsmToken tok) {
		std::cerr << "Parser::calcAddresses() > Duplicated label name \"" << tok.val << "\" in line " << tok.line << ".\n";
		return false;
	}
	return true;
}

ScriptFileHeader Parser::getScriptHeader() {
	return this->m_FileHeader;
}

std::vector<constant> Parser::getNumArray() {
	return this->m_NumArray;
}
