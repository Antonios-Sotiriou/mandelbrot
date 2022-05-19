# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = plot
RM = rm
LINKS = -lX11 -pthread

all:
	$(CC) $(CFLAGS) main.c plot.c iterator.c painter.c threader.c locale.c global_vars.c -o $(OBJ) $(LINKS);
	$(CC) $(CFLAGS) main2.c -o main2 $(LINKS);
exec:

clean:
