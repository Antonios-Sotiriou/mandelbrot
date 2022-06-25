# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -O2 #-Os #-Og #-O0 #-g
OBJ = plot
RM = rm
LINKS = -lX11 -pthread

install:
	./install.sh;

uninstall:
	sudo apt remove libx11-dev;

all:
	$(CC) $(CFLAGS) board.c locale.c -o $(OBJ) $(LINKS);

exec:
	./plot;

clean:
	sudo rm -r ./*;
	sudo apt autoremove -y;

