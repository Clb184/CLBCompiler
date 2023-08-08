#include "Preprocessor.h"

bool Preprocessor::initPreprocessor(std::vector<AsmToken>& tokArray, std::vector<AsmToken>& tokArrayClean, ScriptFileHeader& Header) {
	bool onError = false;
	const size_t size = tokArray.size();
	bool onError = false;
	try {
		for (int i = 0; i < size && !onError; i++) {

			switch (tokArray[i].tok) {
			case ASMTOKEN::ADDRESS:
				break;
			case ASMTOKEN::KEYWORD2: {
				uint32_t cmd = tokArray[i].numval;
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
					tokArrayClean.push_back(tokArray[i]);
					if (cmd != ScriptCommand::OBJCREATE)
						break;
					i++;
					if (tokArray[i].tok == ASMTOKEN::IDENTIFIER2) {
						if (this->m_DefinedTokenMap.find(tokArray[i].val) != this->m_DefinedTokenMap.end()) {
							tokArray[i] = this->m_DefinedTokenMap[tokArray[i].val];
							tokArrayClean.push_back(tokArray[i]);
						}
						else {
							onError = true;
							throw tokArray[i];
						}
					}
					else {
						tokArrayClean.push_back(tokArray[i]);
					}
					break;
				case ScriptCommand::MOVC:
				case ScriptCommand::MOVCS:
				case ScriptCommand::MOVV:
				case ScriptCommand::MOVCE:
				case ScriptCommand::MOVCCN:
					i++;
					if (tokArray[i].tok == ASMTOKEN::IDENTIFIER2) {
						if (this->m_DefinedTokenMap.find(tokArray[i].val) != this->m_DefinedTokenMap.end()) {
							tokArray[i] = this->m_DefinedTokenMap[tokArray[i].val];
							tokArrayClean.push_back(tokArray[i]);
						}
						else {
							onError = true;
							throw tokArray[i];
						}
					}
					else {
						tokArrayClean.push_back(tokArray[i]);
					}
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
				case ScriptCommand::PUSHSTR: {
					bool onRad;
					i++;
					if (onRad = tokArray[i].tok == ASMTOKEN::RAD2) 
						i++;
					if (tokArray[i].tok == ASMTOKEN::IDENTIFIER2) {
						if (this->m_DefinedTokenMap.find(tokArray[i].val) != this->m_DefinedTokenMap.end()) {
							tokArray[i] = this->m_DefinedTokenMap[tokArray[i].val];
							tokArrayClean.push_back(tokArray[i]);
						}
						else {
							onError = true;
							throw tokArray[i];
						}
					}
					else {
						tokArrayClean.push_back(tokArray[i]);
					}
					if (onRad) {
						switch (tokArray[i].tok) {
						case ASMTOKEN::INT_LIT2:
							tokArrayClean[tokArrayClean.size() - 1].numval = (float)(int)tokArrayClean[i].numval;
						case ASMTOKEN::REAL_LIT2:
							tokArrayClean[tokArrayClean.size() - 1].numval = RAD((double)tokArrayClean[i].numval);
							break;
						default:
							onError = true;
							throw tokArrayClean[tokArrayClean.size() - 1];
							break;
						}
					}
				}break;
				}
			}break;

			case ASMTOKEN::PREPROCESSOR2: {
				switch ((int)tokArray[i].numval) {
				case ASMPREPROCESOR::ASM_CONSTANT:
					//Something like #define, done!
					i++;
					if (i + 2 < size &&
						tokArray[i].tok == ASMTOKEN::IDENTIFIER2 &&
						(tokArray[i + 1].tok == ASMTOKEN::INT_LIT2 ||
							tokArray[i + 1].tok == ASMTOKEN::REAL_LIT2)) {
						if (this->m_DefinedTokenMap.find(tokArray[i].val) == this->m_DefinedTokenMap.end())
							this->m_DefinedTokenMap.insert({ tokArray[i].val, tokArray[i + 1] });
						else
							this->m_DefinedTokenMap[tokArray[i].val] = tokArray[i + 1];
						i += 2;
					}
					else {
						throw tokArray[i].numval;
					}
					break;
				case ASMPREPROCESOR::ASM_MUSIC: {
					i++;
					while (i + 3 <= size &&
						tokArray[i].tok == ASMTOKEN::STRING_LIT2 &&
						tokArray[i + 1].tok == ASMTOKEN::INT_LIT2 &&
						tokArray[i + 2].tok == ASMTOKEN::INT_LIT2) {
						Music temp = { tokArray[i].val, (int)tokArray[i + 1].numval, (int)tokArray[i + 2].numval, 0 };
						Header.musc.musicArr.push_back(temp);
						if (tokArray[i + 3].val == ",") {
							i += 4;
							continue;
						}
						else if (tokArray[i + 3].val == ";") {
							i += 4;
							break;
						}
						else {
							throw tokArray[i].numval;
						}
					}
				}break;
				case ASMPREPROCESOR::ASM_SHADER2D:
				case ASMPREPROCESOR::ASM_SHADER3D: {
					int kind = (int)tokArray[i].numval;
					i++;
					if (i + 2 <= size &&
						tokArray[i].tok == ASMTOKEN::STRING_LIT2 &&
						tokArray[i + 1].tok == ASMTOKEN::STRING_LIT2 &&
						tokArray[i + 2].val == ";") {
						if (kind == ASMPREPROCESOR::ASM_SHADER2D) {
							Header.quad.vshader = tokArray[i].val;
							Header.quad.fshader = tokArray[i + 1].val;
						}
						else if (kind == ASMPREPROCESOR::ASM_SHADER3D) {
							Header.m3dl.vshader = tokArray[i].val;
							Header.m3dl.fshader = tokArray[i + 1].val;
						}
						else {
							throw tokArray[i].numval;
							onError = true;
						}
						i += 2;
					}
				}break;
				case ASMPREPROCESOR::ASM_TEXT:
				case ASMPREPROCESOR::ASM_SOUND:
				case ASMPREPROCESOR::ASM_QUAD:
				case ASMPREPROCESOR::ASM_MODEL3D: {
					int kind = (int)tokArray[i].numval;
					i++;
					while (i + 1 < size) {
						if (tokArray[i].tok == ASMTOKEN::STRING_LIT2) {
							if (kind == ASMPREPROCESOR::ASM_QUAD)
								Header.quad.quadArr.push_back(tokArray[i].val);
							else if (kind == ASMPREPROCESOR::ASM_MODEL3D)
								Header.m3dl.modelArr.push_back(tokArray[i].val);
							else if (kind == ASMPREPROCESOR::ASM_SOUND)
								Header.sndm.soundArr.push_back(tokArray[i].val);
							else if (kind == ASMPREPROCESOR::ASM_TEXT)
								Header.StrContainer.push_back(tokArray[i].val);
							if (tokArray[i + 1].val == ",") {
								i += 2;
							}
							else if (tokArray[i + 1].val == ";") {
								i += 2;
								break;
							}
							else {
								throw tokArray[i].numval;
							}
						}
						else {
							throw tokArray[i];
						}
					}
					if (kind == ASMPREPROCESOR::ASM_QUAD)
						Header.quad.size = Header.quad.quadArr.size();
					else if (kind == ASMPREPROCESOR::ASM_MODEL3D)
						Header.m3dl.size = Header.m3dl.modelArr.size();
					else if (kind == ASMPREPROCESOR::ASM_SOUND)
						Header.sndm.size = Header.sndm.soundArr.size();
				}break;
				case ASMPREPROCESOR::ASM_IMPORT: {
					if (i + 1 <= size && tokArray[i + 1].tok == ASMTOKEN::STRING_LIT2) {
						i++;
						//this->appendImport(tokArray[i].val, tokArrayClean.size(), )
					}
					else {
						throw tokArray[i].numval;
						onError = true;
					}

				}break;
				}
			}break;
			}
		}
	}
	catch (AsmToken tok) {
		std::cerr << "Preprocessor::initPreprocessor() > Invalid preprocessor token \"" << tok.val << "\" in line " << tok.line << ".\n";
		return false;
	}
	return true;
}

void Preprocessor::appendImport(std::string incName, size_t pos, std::vector<AsmToken>& tokArray) {
	
}