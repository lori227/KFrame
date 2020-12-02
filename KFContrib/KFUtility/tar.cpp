#include "tar.h"
#define TARHEADER static_cast<PosixTarHeader*>(header)

Tar::Tar(const char *filename): _finished(false){
	this->out = std::fopen(filename, "wb");
	if(out == NULL){
		throw( "Cannot open output");
	}
	this->_closeFile = true;
	this->_finished  = true;
    if(sizeof(PosixTarHeader)!=512){
		throw(sizeof(PosixTarHeader));
	}
}

Tar::Tar(std::FILE *file): _finished(false){
	this->out = file;
	if(out == NULL){
		throw( "Cannot open output");
	}
	this->_closeFile = false;
	this->_finished  = true;
    if(sizeof(PosixTarHeader)!=512){
		throw(sizeof(PosixTarHeader));
	}
}

Tar::~Tar(){
    if(!_finished){
		throw("Tar file was not finished.");
	}
}

void Tar::_init(void* header){
    std::memset(header,0,sizeof(PosixTarHeader));
    std::sprintf(TARHEADER->magic,"ustar  ");
    std::sprintf(TARHEADER->mtime, "%011lo", time(NULL));
    std::sprintf(TARHEADER->mode, "%07o", 0777);
    std::sprintf(TARHEADER->gname,"%s","users");
}

void Tar::_checksum(void* header){
    unsigned int sum = 0;
    char *p = (char *) header;
    char *q = p + sizeof(PosixTarHeader);
    while (p < TARHEADER->checksum) sum += *p++ & 0xff;
    for (int i = 0; i < 8; ++i)  {
	  sum += ' ';
	  ++p;
	}
    while (p < q) sum += *p++ & 0xff;

    std::sprintf(TARHEADER->checksum,"%06o",sum);
}

void Tar::_size(void* header,unsigned long fileSize){
    std::sprintf(TARHEADER->size,"%011llo",(long long unsigned int)fileSize);
}

void Tar::_filename(void* header,const char* filename){
    if(filename==NULL || filename[0]==0 || std::strlen(filename)>=100){
	//throw("invalid archive name \"" << filename << "\"");
		throw(20);
	}
    sprintf(TARHEADER->name,"%s",filename); // max 100 chars !!!
}

void Tar::_endRecord(std::size_t len){
    char c='\0';
    while((len%sizeof(PosixTarHeader))!=0){
		std::fwrite(&c, sizeof(char), sizeof(char), out);
		++len;
	}
}

void Tar::close(){
	if(!_finished){
		_finished=true;
		PosixTarHeader header;
		std::memset((void*)&header, 0, sizeof(PosixTarHeader));
		std::fwrite((const char*)&header, sizeof(char), sizeof(PosixTarHeader), out);
		std::fwrite((const char*)&header, sizeof(char), sizeof(PosixTarHeader), out);
	}
	if(this->_closeFile) std::fclose(this->out);
}

void Tar::put(const char* filename, const std::string& s){
    put(filename, s.c_str(), s.size());
}

void Tar::put(const char* filename, const char* content){
    put(filename, content, std::strlen(content));
}

void Tar::put(const char* filename,const char* content,std::size_t len){
    PosixTarHeader header;
    _init((void*)&header);
    _filename((void*)&header, filename);
    header.typeflag[0]=48;
    _size((void*)&header, len);
    _checksum((void*)&header);
    std::fwrite((const char*)&header, sizeof(char), sizeof(PosixTarHeader), out);
    std::fwrite(content, sizeof(char), len, out);
    _endRecord(len);
}

void Tar::putFile(const char* filename, const char* nameInArchive){

    std::FILE* in = std::fopen(filename, "rb");
    if(in == NULL){
		//throw("Cannot open " << filename << " "<< std::strerror(errno));
		throw(10);
	}

    std::fseek(in, 0L, SEEK_END);
    long int len = std::ftell(in);
    std::fseek(in, 0L, SEEK_SET);

    PosixTarHeader header;
    _init((void*)&header);
    _filename((void*)&header,nameInArchive);
    header.typeflag[0]=0;
    _size((void*)&header, len);
    _checksum((void*)&header);
    std::fwrite((const char*)&header, sizeof(char), sizeof(PosixTarHeader), out);

	char buff[BUFSIZ];
	unsigned long int total = 0;
    std::size_t nRead=0;
    while( ( nRead = std::fread(buff ,sizeof(char), BUFSIZ, in) ) > 0) {
		std::fwrite(buff, sizeof(char), nRead, out);
		total = total + nRead;
	}
    std::fclose(in);

    _endRecord(total);
}

long int Tar::fileLength(std::FILE *file){
	std::fseek(file, 0L, SEEK_END);
    long int len = std::ftell(file);
    std::fseek(file, 0L, SEEK_SET);
	return len;
}