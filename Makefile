# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = plot
RM = rm
LINKS = -lX11 -pthread

all:
	$(CC) $(CFLAGS) plot.c iterator.c -o $(OBJ) $(LINKS);
	$(CC) $(CFLAGS) plot2.c painter.c threader.c -o plot2 $(LINKS);
exec:

clean:
