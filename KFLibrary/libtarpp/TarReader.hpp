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
	class TarReader
	{
		vector<TarContents> contents;

		public:

		void load(const string& filename);

		string readText(const string& path);
		vector<uint8_t> readBinary(const string& path);
		private:
		TarContents getContents(const string& path);
	};
}
