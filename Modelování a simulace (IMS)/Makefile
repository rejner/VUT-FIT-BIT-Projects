PROJ=ims_model
CC=g++
CFLAGS=-Wall -Wextra -pedantic -lsimlib

SOURCE_FILES=main.cpp main.h

all: 
	cd src; \
	$(CC) $(SOURCE_FILES) -o ../ims_model $(CFLAGS)
	
clean:
	rm -f *.o *.out $(PROJ)
    
run: ./ims_model
	./ims_model
