#pragma once
#include <string>

#ifdef __linux__
#include <pwd.h>
#endif

#include <vector>
#include "TarContents.hpp"
using namespace std;

namespace libtarpp
{
	class TarWriter
	{
		vector<TarContents> contents;

		public:

		//void load(const string filename);
		void save(const string& filename);
		void addFile(const string& filename);
		void addFile(const string& filename,const string& path);
		void addText(const string& text,const string& path);
		void addBinary(const vector<uint8_t>& data,const string& path);
		//TarContents getContents(const string filename);
	};
}
