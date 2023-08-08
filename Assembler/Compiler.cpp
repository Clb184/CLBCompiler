#include "Compiler.h"

Compiler::Compiler() {

}

void Compiler::Initialize(const std::string& in, const std::string& out) {
	this->m_OutName = out;
	this->m_Parser.Initialize(in);
	this->m_Output.open(out, std::ios::binary | std::ios::out);
}

void Compiler::compileFile() {
	FILE* pFile = nullptr;
	char* pBuffer = nullptr;
	
	if (this->m_Parser.parseTokens()) {
		this->m_Header = this->m_Parser.getScriptHeader();
		this->m_NumArr = this->m_Parser.getNumArray();

		const size_t size = this->calculateSize();
		pBuffer = (char*)std::malloc(size);
		std::memset(pBuffer, 0x00, size);
		this->writeBuffer(pBuffer);
		pFile = std::fopen(this->m_OutName.c_str(), "wb");
		fwrite(pBuffer, size, 1, pFile);
		fclose(pFile);
		std::free(pBuffer);
	}
}

size_t Compiler::calculateSize() {
	int i = 0;
	size_t finalOutSize = 0;

	for (auto& s : this->m_Header.StrContainer) {
		this->m_GlobalStrContainer.push_back(s);
		if (this->m_stringMap.find(s) == this->m_stringMap.end())
			this->m_stringMap.insert({ s, i++ });
	}
	for (auto& s : this->m_Header.sndm.soundArr) {
		this->m_GlobalStrContainer.push_back(s);
		if (this->m_stringMap.find(s) == this->m_stringMap.end()) {
			this->m_stringMap.insert({ s, i++ });
		}
	}for (auto& s : this->m_Header.musc.musicArr) {
		this->m_GlobalStrContainer.push_back(s.stringID);
		if (this->m_stringMap.find(s.stringID) == this->m_stringMap.end())
			this->m_stringMap.insert({ s.stringID, i++ });
	}
	if (this->m_stringMap.find(this->m_Header.quad.vshader) == this->m_stringMap.end()) {
		this->m_GlobalStrContainer.push_back(this->m_Header.quad.vshader);
		this->m_stringMap.insert({ this->m_Header.quad.vshader, i++ });
	}
	if (this->m_stringMap.find(this->m_Header.quad.fshader) == this->m_stringMap.end()) {
		this->m_GlobalStrContainer.push_back(this->m_Header.quad.fshader);
		this->m_stringMap.insert({ this->m_Header.quad.fshader, i++ });
	}
	for (auto& s : this->m_Header.quad.quadArr) {
		this->m_GlobalStrContainer.push_back(s);
		if (this->m_stringMap.find(s) == this->m_stringMap.end())
			this->m_stringMap.insert({ s, i++ });
	}
	if (this->m_stringMap.find(this->m_Header.m3dl.vshader) == this->m_stringMap.end()) {
		this->m_GlobalStrContainer.push_back(this->m_Header.m3dl.vshader);
		this->m_stringMap.insert({ this->m_Header.m3dl.vshader, i++ });
	}
	if (this->m_stringMap.find(this->m_Header.m3dl.fshader) == this->m_stringMap.end()) {
		this->m_GlobalStrContainer.push_back(this->m_Header.m3dl.fshader);
		this->m_stringMap.insert({ this->m_Header.m3dl.fshader, i++ });
	}
	for (auto& s : this->m_Header.m3dl.modelArr) {
		this->m_GlobalStrContainer.push_back(s);
		if (this->m_stringMap.find(s) == this->m_stringMap.end())
			this->m_stringMap.insert({ s, i++ });
	}

	for (auto& it : this->m_stringMap) {
		finalOutSize += it.first.size() + 1;
	}
	finalOutSize++;
	size_t size;
	size = this->m_Header.sndm.soundArr.size();
	this->m_HeaderSub.push_back(MakeCStr('s', 'n', 'd', 'm'));
	this->m_HeaderSub.push_back(size);
	for (size_t x = 0; x < size; x++) {
		this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.sndm.soundArr[x]]);
	}
	size = this->m_Header.musc.musicArr.size();
	this->m_HeaderSub.push_back(MakeCStr('m', 'u', 's', 'c'));
	this->m_HeaderSub.push_back(size);
	for (size_t x = 0; x < size; x++) {
		this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.musc.musicArr[x].stringID]);
		this->m_HeaderSub.push_back(this->m_Header.musc.musicArr[x].loopStart);
		this->m_HeaderSub.push_back(0);
		this->m_HeaderSub.push_back(this->m_Header.musc.musicArr[x].loopEnd);
		this->m_HeaderSub.push_back(0);
	}
	size = this->m_Header.quad.quadArr.size();
	this->m_HeaderSub.push_back(MakeCStr('s', 'h', '2', 'd'));
	this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.quad.vshader]);
	this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.quad.fshader]);
	this->m_HeaderSub.push_back(MakeCStr('q', 'u', 'a', 'd'));
	this->m_HeaderSub.push_back(size);
	for (size_t x = 0; x < size; x++) {
		this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.quad.quadArr[x]]);
	}
	size = this->m_Header.m3dl.modelArr.size();
	this->m_HeaderSub.push_back(MakeCStr('s', 'h', '3', 'd'));
	this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.m3dl.vshader]);
	this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.m3dl.fshader]);
	this->m_HeaderSub.push_back(MakeCStr('m', '3', 'd', 'l'));
	this->m_HeaderSub.push_back(size);
	for (size_t x = 0; x < size; x++) {
		this->m_HeaderSub.push_back(this->m_stringMap[this->m_Header.m3dl.modelArr[x]]);
	}

	return finalOutSize += ((this->m_HeaderSub.size() + this->m_NumArr.size()) << 2) + 1;
}

void Compiler::writeBuffer(char* buffer) {
	size_t pos = 0;
	for (auto& s: this->m_GlobalStrContainer) {
		std::memcpy(buffer + pos, s.c_str(), s.size());
		pos += s.size() + 1;
	}
	buffer[pos++] = 0x00;
	for (auto& h : this->m_HeaderSub) {
		std::memcpy(buffer + pos, &h, sizeof(uint32_t));
		pos += sizeof(uint32_t);
	}
	buffer[pos++] = 0x00;
	for (auto& n : this->m_NumArr) {
		std::memcpy(buffer + pos, &n, sizeof(constant));
		pos += sizeof(constant);
	}
}