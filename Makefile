# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = plot
RM = rm
LINKS = -lX11

all:
	$(CC) $(CFLAGS) plot.c iterator.c painter.c -o $(OBJ) $(LINKS)
exec:

clean:
