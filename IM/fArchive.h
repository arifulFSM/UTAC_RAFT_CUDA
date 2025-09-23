#pragma once

#include <fstream>

class CfArchive {
	std::fstream* f = nullptr;

public:
	void w(bool v) { f->write(reinterpret_cast<char*>(&v), sizeof(bool)); }
	void r(bool& v) { f->read(reinterpret_cast<char*>(&v), sizeof(bool)); }

	void w(char v) { f->write(reinterpret_cast<char*>(&v), sizeof(char)); }
	void r(char& v) { f->read(reinterpret_cast<char*>(&v), sizeof(char)); }

	void w(short v) { f->write(reinterpret_cast<char*>(&v), sizeof(short)); }
	void r(short& v) { f->read(reinterpret_cast<char*>(&v), sizeof(short)); }
	void w(unsigned short v) { f->write(reinterpret_cast<char*>(&v), sizeof(unsigned short)); }
	void r(unsigned short& v) { f->read(reinterpret_cast<char*>(&v), sizeof(unsigned short)); }

	void w(int v) { f->write(reinterpret_cast<char*>(&v), sizeof(int)); }
	void r(int& v) { f->read(reinterpret_cast<char*>(&v), sizeof(int)); }
	void w(size_t v) { f->write(reinterpret_cast<char*>(&v), sizeof(size_t)); }
	void r(size_t& v) { f->read(reinterpret_cast<char*>(&v), sizeof(size_t)); }
	void w(unsigned int v) { f->write(reinterpret_cast<char*>(&v), sizeof(unsigned int)); }
	void r(unsigned int& v) { f->read(reinterpret_cast<char*>(&v), sizeof(unsigned int)); }

	void w(long v) { f->write(reinterpret_cast<char*>(&v), sizeof(long)); }
	void r(long& v) { f->read(reinterpret_cast<char*>(&v), sizeof(long)); }
	void w(unsigned long v) { f->write(reinterpret_cast<char*>(&v), sizeof(unsigned long)); }
	void r(unsigned long& v) { f->read(reinterpret_cast<char*>(&v), sizeof(unsigned long)); }

	void w(float v) { f->write(reinterpret_cast<char*>(&v), sizeof(float)); }
	void r(float& v) { f->read(reinterpret_cast<char*>(&v), sizeof(float)); }
	void w(double v) { f->write(reinterpret_cast<char*>(&v), sizeof(double)); }
	void r(double& v) { f->read(reinterpret_cast<char*>(&v), sizeof(double)); }

	void w(std::string v) { *f << v << std::endl; }
	void r(std::string& v) { *f >> v; char c; r(c); }

public:
	CfArchive(std::fstream* fp) : f(fp) {}
};
