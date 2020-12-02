#include "TarWriter.hpp"
#include "TarContents.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>

#ifdef __linux__
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#endif
using namespace std;

void libtarpp::TarWriter::addFile(const string& filename)
{
	int pos = filename.find_last_of("/");
	addFile(filename,filename.substr(pos+1,filename.size()-pos));
}

void libtarpp::TarWriter::addFile(const string& filename,const string& path)
{
	
	auto it = contents.begin();
	auto ct = TarContents();
	ct.setName(path);

	struct stat s;
	if(stat(filename.c_str(),&s)!=0)
	{
		throw "couldn't stat file:"+filename;
	}

	ostringstream oss_mode;
	oss_mode<<oct<<s.st_mode<<flush;
	ct.setMode(oss_mode.str());
	
	ostringstream oss_uid;
	oss_uid<<s.st_uid<<flush;
	ct.setUid(oss_uid.str());
	
	ostringstream oss_gid;
	oss_gid<<s.st_gid<<flush;
	ct.setGid(oss_gid.str());

	ostringstream oss_size;
	oss_size<<oct<<s.st_size<<flush;
	ct.setRawSize(oss_size.str());
	

	ostringstream oss_mtime;
	oss_mtime<<oct<<s.st_mtime<<flush;
	ct.setMTime(oss_mtime.str());


	ct.setTypeFlag("0");
	bool link = false;
	if(S_ISLNK(s.st_mode))
	{
		ct.setTypeFlag("1");
		link = true;
	}
	if(S_ISCHR(s.st_mode))
	{
		ct.setTypeFlag("3");
	}
	if(S_ISBLK(s.st_mode))
	{
		ct.setTypeFlag("4");
	}
	if(S_ISDIR(s.st_mode))
	{
		ct.setTypeFlag("5");
	}
	if(S_ISFIFO(s.st_mode))
	{
		ct.setTypeFlag("6");
	}
	if(link)
	{
		char buf[100];
		readlink(filename.c_str(),buf,sizeof(buf));
		string tmp(buf);
		ct.setLinkName(tmp);
	}

	struct passwd *p;
	p=getpwuid(s.st_uid);
	ostringstream oss_uname;
	//oss_uname<<s.st_uname<<flush;
	ct.setUName(p->pw_name);
	
	struct group *g;
	g=getgrgid(s.st_gid);
	ct.setGName(g->gr_name);

	ct.autoChkSum();

	ct.setStream(shared_ptr<fstream>(new fstream(filename,ios_base::in)));
	contents.push_back(ct);
	
}

void libtarpp::TarWriter::addText(const string& text,const string& path)
{
	TarContents t;
	t.setName(path);
	t.setMode("010777");
	#ifdef __linux__
	struct passwd *pw;
	pw = getpwuid(getuid());
	t.setUid(to_string(pw->pw_uid));
	t.setGid(to_string(pw->pw_gid));
	t.setUName(pw->pw_name);
	struct group *g;
	g = getgrgid(getgid());
	t.setGName(g->gr_name);
	#else
	#endif
	
	ostringstream oss_size;
	oss_size<<oct<<text.size()<<flush;
	//t.setSize(oss_size.str());
	t.setRawSize(oss_size.str());
	shared_ptr<stringstream> s(new stringstream());
	*s<<text<<flush;

	t.autoChkSum();
	t.setStream(s);

	contents.push_back(t);
	//cout<<contents.size() <<endl;
}

void libtarpp::TarWriter::save(const string& filename)
{
	ofstream ofs(filename);
	for(auto it:contents)
	{
		//cout<<"processing:" +it.getName()<<endl;
		cout<<"libtarpp: saving "+it.getName()<<endl;
		ofs.write(it.getName().data(),it.getName().size());
		ofs<<it.getMode()<<flush;
		ofs<<it.getUid()<<flush;
		ofs<<it.getGid()<<flush;
		ofs<<it.getRawSize()<<flush;
		ofs<<it.getMTime()<<flush;
		ofs<<it.getChkSum()<<flush;
		ofs<<it.getTypeFlag()<<flush;
		ofs<<it.getLinkName()<<flush;
		ofs<<it.magic<<flush;
		ofs<<it.version<<flush;
		ofs<<it.getUName()<<flush;
		ofs<<it.getGName()<<flush;
		ofs<<it.getDevMajor()<<flush;
		ofs<<it.getDevMinor()<<flush;
		ofs<<it.getPrefix()<<flush;
		for(int i=1; i<=12; i++)
		{
			ofs<<'\0';
		}
		ofs<<flush;
		if(it.getSize()==0)
		{
			//cout<<"0"<<endl;
			continue;
		}
		ofs << (it.getStream())->rdbuf()<<flush;
		long size = it.getSize();

	

		for(int i=1; i<=512-size%512;i++)
		{
			ofs<<'\0';
		}
		//cout<<"finished:"+it.getName()<<endl;
	}

		for(int i=1; i<=512*2;i++)
		{
			ofs<<'\0';
		}
		ofs<<flush;

		ofs.close();
}

void libtarpp::TarWriter::addBinary(const vector<uint8_t>& data,const string& path)
{
	ostringstream ss;
	for(size_t i = 0; i<data.size();i++)
	{
		ss<<(char) data[i];
	}
	ss<<flush;
	addText(ss.str(),path);
}
/*
int main(void)
{
	libtarpp::TarArchive ta;
	//ta.addFile("./TarArchive.cpp");
	//ta.addFile("./TarArchive.hpp");
	//ta.save("untisitai.tar");

	ta.load("untisitai.tar");
	cout<<(char)(ta.getContents("TarWriter.hpp").getStream()->rdbuf()->sgetc())<<endl;
	//cout<<""<<endl;
	//cout<<hex<<ta.getContents("TarArchive.cpp").getStream()<<endl;
	cout<<ta.getContents("TarWriter.hpp").getName()<<endl;
	libtarpp::TarArchive t2;
	t2.addFile("./tntn");
	t2.save("unko.tar");

}

*/
