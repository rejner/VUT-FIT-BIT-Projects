# ICP projekt
# Aplikace na simulaci mhd
# Vytvorili: Michal Rein, Ondrej Motycka

all: update
	make -f qmakefile

run: 
	./ICP-Project


update: src/ICP-Project.pro
	qmake -o qmakefile src/ICP-Project.pro

doxygen:
	doxygen doc/Doxyfile

clean:
	rm -f ICP-Project *.o *.h *.cpp qmakefile

pack: 
	zip -r xreinm00-xmotyc07.zip src doc/Doxyfile examples Makefile README.txt
