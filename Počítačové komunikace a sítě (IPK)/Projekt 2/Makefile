PROJ=ipk-sniffer
CC=g++
CFLAGS=-lpcap

SOURCE_FILES=main.cpp main.h

all: main.cpp
	$(CC) $(SOURCE_FILES) -o ipk-sniffer $(CFLAGS)

clean:
	rm -f *.o *.out $(PROJ)
