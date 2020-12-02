#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>

using namespace std;
namespace libtarpp
{
	class TarContents
	{
		string name;
		string mode;
		string uid;
		string gid;
		string size;
		string mtime;
		string chksum;
		string typeflag;
		//int lflag;
		string linkname;
		public:
		static const string magic;
		static const string version;
		private:
		string uname;
		string gname;
		string devmajor;
		string devminor;
		string prefix;


		shared_ptr<istream> stream;

		bool isOctal(const string numstring);
		bool isNum(const string numstring);
		string regulateUGId(const string id);
		public:
		TarContents();
		string getName();
		void setName(const string name);
		string getMode();
		void setMode(const string m);
		string getUid();
		void setUid(const string id);
		string getGid();
		void setGid(const string id);
		
		size_t getSize();
		void setSize(const size_t& sz);
		string getRawSize();
		void setRawSize(const string s);

		string getMTime();
		void setMTime(const string m);
		string getChkSum();
		void setChkSum(const string c);
		void autoChkSum();
		string getTypeFlag();
		void setTypeFlag(const string f);
		string getLinkName();
		void setLinkName(const string ln);
		string getUName();
		void setUName(const string un);
		string getGName();
		void setGName(const string gn);
		string getDevMajor();
		void setDevMajor(const string dma);
		string getDevMinor();
		void setDevMinor(const string dmi);
		string getPrefix();
		void setPrefix(const string p);
		shared_ptr<istream> getStream();
		void setStream(shared_ptr<istream> s);
	};
}
