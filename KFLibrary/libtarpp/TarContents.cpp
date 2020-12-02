#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <numeric>
#include "TarContents.hpp"
using namespace std;
libtarpp::TarContents::TarContents()
{
	setName("");
	setMode("0100777");
	setUid("0");
	setGid("0");
	setRawSize("0");
	setMTime("0");
	setTypeFlag("0");
	setLinkName("");
	setUName("root");
	setGName("root");
	setDevMajor("0");
	setDevMinor("0");
	setPrefix("");
}

const string libtarpp::TarContents::magic = "ustar ";
const string libtarpp::TarContents::version = string{' ','\0'};

bool libtarpp::TarContents::isOctal(const string numstring)
{
	for(auto c: numstring)
	{
		if(
		c=='0' ||
		c=='1' ||
		c=='2' ||
		c=='3' ||
		c=='4' ||
		c=='5' ||
		c=='6' ||
		c=='7'
		)
		{
			continue;
		}else{
			return false;
		}
	}
	return true;
}

bool libtarpp::TarContents::isNum(const string numstring)
{
	for(auto c:numstring)
	{
		for(int i=0;i<=9;i++)
		{
			if(c == to_string(i)[0])
			{
				break;
			}else{
				if(i==9)
				{
					return false;
				}
			}
		}
	}
	return true;
}

void libtarpp::TarContents::setName(const string x)
{
	if(x.size()>100)
	{
		throw "error: The length of filename must be less than 100.";
	}
	
	string tmp;
	tmp+=x;
	for(int i=1; i<=100 - x.size();i++)
	{
		tmp+='\0';
	}
	name = tmp;
}

string libtarpp::TarContents::getName()
{
	return name;
}

void libtarpp::TarContents::setMode(const string m)
{
	//cout<<m.size() << endl;
	if(m.size() == 8)
	{
		if(isOctal(m.substr(0,7)))
		{
			mode = m;
		}
	}
	if(m.size() == 7)
	{
		if(isOctal(m))
		{
			mode = m;
			mode += '\0';
		}
	}
	if(m.size() <=6)
	{
		if(isOctal(m))
		{
			string tmp = m;
			for(int i = 1;i<=8-m.size()-1;i++)
			{
				tmp = "0" + tmp;
			}
			tmp += '\0';
			mode = tmp;
		}
	}

}

string libtarpp::TarContents::getMode()
{
	return mode;
}

string libtarpp::TarContents::regulateUGId(const string id)
{
	if(id.size() == 8 && isNum(id.substr(0,7)) )
	{
		return id;
	}
	if(id.size() == 7 && isNum(id))
	{
		string tmp;
		tmp = id;
		tmp += '\0';
		return tmp;
	}
	if(id.size() <=6 && isNum(id))
	{
		string tmp;
		tmp = id;
		for(int i = 1;i<=8-id.size()-1;i++)
		{
			tmp = "0" + tmp;
		}
		tmp += '\0';
		return tmp;
	}

	throw "Invalid uid/gid.";
}


string libtarpp::TarContents::getUid()
{
	return uid;
}

void libtarpp::TarContents::setUid(const string u)
{
	uid = regulateUGId(u);
}

string libtarpp::TarContents::getGid()
{
	return gid;
}

void libtarpp::TarContents::setGid(const string g)
{
	gid = regulateUGId(g);
}

size_t libtarpp::TarContents::getSize()
{
	return (size_t)stoi(getRawSize().substr(0,11),nullptr,8);
}

void libtarpp::TarContents::setSize(const size_t& sz)
{
	ostringstream oss;
	oss<<oct<<sz;
	string str = oss.str();
	if(str.size() < 12)
	{
		for(int i = 1; i<=(11-str.size());i++)
		{
			str="0" + str;
		}
		str+='\0';
		setRawSize(str);
	}
	throw "error!";
}

string libtarpp::TarContents::getRawSize()
{
	return size;
}

void libtarpp::TarContents::setRawSize(const string s)
{
	if(s.size() == 12 && isOctal(s.substr(0,11)))
	{
		size = s;
	}
	if(s.size() == 11 && isOctal(s))
	{
		string tmp;
		tmp = s;
		tmp += '\0';
		size = tmp;
	}
	if(s.size()<=10 && isOctal(s))
	{
		string tmp;
		tmp = s;
		for(int i=1; i<=12-s.size()-1;i++)
		{
			tmp = "0" + tmp;
		}
		tmp += '\0';
		size = tmp;
	}
}
//We need to fix following by 2038!!
string libtarpp::TarContents::getMTime()
{
	return mtime;
}

void libtarpp::TarContents::setMTime(const string m)
{
	if(m.size() == 12 && isNum(m.substr(0,11)))
	{
		mtime = m;
	}
	if(m.size() == 11 && isNum(m))
	{
		string tmp;
		tmp = m;
		tmp += '\0';
		mtime = tmp;
	}
	if(m.size()<=10 && isNum(m))
	{
		string tmp;
		tmp = m;
		for(int i=1; i<=12-m.size()-1;i++)
		{
			tmp = "0" + tmp;
		}
		tmp += '\0';
		mtime = tmp;
	}
}

string libtarpp::TarContents::getChkSum()
{
	return chksum;
}

void libtarpp::TarContents::setChkSum(const string c)
{
	if(c.size() == 8 && isNum(c.substr(0,6)) )
	{
		chksum=c;
		return;
	}
	if(c.size() == 7 && isNum(c))
	{
		string tmp;
		tmp = c;
		tmp += '\0';
		chksum=tmp;
		return;
	}
	if(c.size() <=6 && isNum(c))
	{

		//cout<<"あ"<<endl;
		string tmp;
		tmp = c;
		for(int i = 1;i<=8-c.size()-2;i++)
		{
			tmp = "0" + tmp;
		}
		tmp += '\0';
		tmp += ' ';
		chksum=tmp;
		return;
	}
	/*
	cout<<c.size() <<endl;
	cout<<c << endl;
	*/
	throw "Invalid chksum.";

}

void libtarpp::TarContents::autoChkSum()
{
	if(
	getName().size() == 100 &&
	getMode().size() == 8 &&
	getUid().size() == 8 &&
	getGid().size() == 8 &&
	getRawSize().size() == 12 &&
	getMTime().size() == 12 &&
	getTypeFlag().size() == 1 &&
	magic.size() == 6 &&
	version.size() == 2 &&
	getUName().size() == 32 &&
	getGName().size() == 32 &&
	getDevMajor().size() == 8 &&
	getDevMinor().size() == 8 &&
	getPrefix().size() == 155
	)
	{
		unsigned int sum = 0;
		for(char x:getName())
		{
			sum +=x;
		}
		for(char x:getMode())
		{
			sum +=x;
		}
		for(char x:getUid())
		{
			sum +=x;
		}
		for(char x:getGid())
		{
			sum +=x;
		}
		for(char x:getRawSize())
		{
			sum +=x;
		}
		for(char x:getMTime())
		{
			sum +=x;
		}
		for(char x:getTypeFlag())
		{
			sum +=x;
		}
		for(char x:getLinkName())
		{
			sum +=x;
		}
		for(char x:magic)
		{
			sum +=x;
		}
		for(char x:version)
		{
			sum +=x;
		}
		for(char x:getUName())
		{
			sum +=x;
		}
		for(char x:getGName())
		{
			sum +=x;
		}
		for(char x:getDevMajor())
		{
			sum +=x;
		}
		for(char x:getDevMinor())
		{
			sum +=x;
		}
		for(char x:getPrefix())
		{
			sum +=x;
		}
		sum+=32*8;
		/*
		accumulate(getUid().begin(),getUid().end(),0)+
		accumulate(getGid().begin(),getGid().end(),0)+
		accumulate(getSize().begin(),getSize().end(),0)+
		accumulate(getMTime().begin(),getMTime().end(),0)+
		accumulate(getTypeFlag().begin(),getTypeFlag().end(),0)+
		accumulate(magic.begin(),magic.end(),0)+
		accumulate(version.begin(),version.end(),0)+
		accumulate(getUName().begin(),getUName().end(),0)+
		accumulate(getGName().begin(),getGName().end(),0)+
		accumulate(getDevMajor().begin(),getDevMajor().end(),0)+
		accumulate(getDevMinor().begin(),getDevMinor().end(),0)+
		accumulate(getPrefix().begin(),getPrefix().end(),0);*/
		//cout<<oct<<sum<<endl;
		ostringstream oss_chksum;
		oss_chksum<<oct<<sum<<flush;
		setChkSum(oss_chksum.str());
	}else{
		throw "Can't calculate chksum: broken header.";
	}
}

string libtarpp::TarContents::getTypeFlag()
{
	return typeflag;
}

void libtarpp::TarContents::setTypeFlag(const string f)
{
	if(f.size() == 1)
	{
		typeflag = f;
		return;
	}
	throw "error: invalid typeflag.";
}

string libtarpp::TarContents::getLinkName()
{
	return linkname;
}

void libtarpp::TarContents::setLinkName(const string ln)
{
	if(ln.size() == 100)
	{
		linkname = ln;
		return;
	}
	if(ln.size()<100)
	{
		string tmp;
		tmp = ln;
		for(int i=1;i<=100-ln.size();i++)
		{
			tmp += '\0';
		}
		linkname = tmp;
		return;
	}
}

string libtarpp::TarContents::getUName()
{
	return uname;
}

void libtarpp::TarContents::setUName(const string un)
{
	if(un.size() == 32 && un[31] == '\0')
	{
		uname =un;
		return;
	}
	if(un.size() <= 31)
	{
		string tmp;
		tmp = un;
		for(int i = 1;i<=32-un.size();i++)
		{
			tmp +='\0';
		}
		uname = tmp;
		return;
	}
	throw "Too long uname.";
}

string libtarpp::TarContents::getGName()
{
	return gname;
}

void libtarpp::TarContents::setGName(const string gn)
{
	if(gn.size() == 32 && gn[31] == '\0')
	{
		gname =gn;
		return;
	}
	if(gn.size() <= 31)
	{
		string tmp;
		tmp = gn;
		for(int i = 1;i<=32-gn.size();i++)
		{
			tmp +='\0';
		}
		gname = tmp;
		return;
	}
	throw "Too long uname.";

}

string libtarpp::TarContents::getDevMajor()
{
	return devmajor;
}

void libtarpp::TarContents::setDevMajor(const string dma)
{
	string tmp;
	for(int i=1; i<=8;i++)
	{
		tmp+='\0';
	}
	devmajor = tmp;
}

string libtarpp::TarContents::getDevMinor()
{
	return devminor;
}

void libtarpp::TarContents::setDevMinor(const string dmi)
{
	string tmp;
	for(int i=1; i<=8;i++)
	{
		tmp+='\0';
	}
	devminor = tmp;

}

string libtarpp::TarContents::getPrefix()
{
	return prefix;
}

void libtarpp::TarContents::setPrefix(const string p)
{
	if(p.size()==155)
	{
		prefix = p;
		return;
	}
	if(p.size()<=154)
	{
		string tmp;
		for(int i=1;i<=155-p.size();i++)
		{
			tmp += '\0';
		}
		prefix = tmp;
		return;
	}
}

shared_ptr<istream> libtarpp::TarContents::getStream()
{
	return stream;
}

void libtarpp::TarContents::setStream(shared_ptr<istream> s)
{
	stream = s;
}
