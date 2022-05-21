# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = plot
RM = rm
LINKS = -lX11 -pthread

all:
	$(CC) $(CFLAGS) main.c board.c transmitter.c locale.c global_vars.c -o $(OBJ) $(LINKS);
	$(CC) $(CFLAGS) main2.c threader.c painter.c -o main2 $(LINKS);
exec:

clean:
