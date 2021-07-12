PROJ=discord_bot
CC=g++
CFLAGS=-Wall -Wextra -pedantic -lcrypto -lssl

SOURCE_FILES=main.cpp main.h message.cpp message.h

all: main.cpp
	$(CC) $(SOURCE_FILES) -o isabot $(CFLAGS)
	
clean:
	rm -f *.o *.out $(PROJ)
