#pragma once
#include <string>

using namespace std;
namespace libtarpp
{
	class TarHeader
	{
		string name;
		string mode;
		string uid;
		string gid;
		string size;
		string mtime;
		string checksum;
		int lflag;
		string lname;
		static const string magic;
		static const string version;
		string uname;
		string gname;
		string devmajor;
		string devminor;
		public:
		void setName(string name);
		void setMode(string m);
		void setUid(string id);

		TarHeader();
	};
}
