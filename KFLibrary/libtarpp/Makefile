CC=g++
CFLAGS=-Wall -std=c++11 -g -fPIC -O0
OUTPUT=libtarpp.a
OBJS=TarArchive.o TarContents.o TarWriter.o TarReader.o
all: $(OBJS)
	ar rcs $(OUTPUT) $(OBJS) $(LIBTARPP)
#CCellDefinition.o:CCellDefinition.cpp CCellDefinition.hpp
#	$(CC) $(CFLAGS) -c $< -I/home/carrot031/libtarpp
#CCellDefinition.o:CCell.cpp CCell.hpp
#	$(CC) $(CFLAGS) -c $< -I/home/carrot031/libtarpp
%.o:%.cpp %.hpp
	$(CC) $(CFLAGS) -c $< $(INCLUDE)
#Size.o:Size.cpp Size.hpp
#	$(CC) -std=c++11 -c Size.cpp
#Point.o:Point.cpp Point.hpp
#	$(CC) -std=c++11 -c Point.cpp

clean:
	rm -f $(OBJS) $(OUTPUT)

