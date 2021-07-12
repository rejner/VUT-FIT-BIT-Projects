
NAME = proj2
EXEC = $(NAME)
sources = $(NAME).c

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread -std=gnu99 -pedantic
LDFLAGS = -lrt 
OBJ = $(NAME).o

.PHONY : clean

%.o : %.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o core *.out

cleanall: clean
	rm -f $(EXEC)
