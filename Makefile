# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = plot
RM = rm
LINKS = -lX11 -pthread #-lm

install:
	./install.sh;

uninstall:
	sudo apt remove libx11-dev;

all:
	$(CC) $(CFLAGS) main.c locale.c board.c transmitter.c shmem.c procsync.c external_vars.c -o $(OBJ) $(LINKS);
	$(CC) $(CFLAGS) main2.c threader.c painter.c shmem.c procsync.c -o main2 $(LINKS);

exec:
	./plot

clean:
	sudo apt autoremove -y;

