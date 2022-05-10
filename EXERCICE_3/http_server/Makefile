adresse_dir = ../adresse_internet/
socket_dir = ../socket_tcp/
CC = gcc
CPPFLAGS = -D_XOPEN_SOURCE=700
CFLAGS = -std=c18 -Wpedantic -Wall -Wextra -Wconversion -Wwrite-strings -Werror -fstack-protector-all -fpie -ld -O2 -g -I $(adresse_dir) -I $(socket_dir) 
LDFLAGS =

vpath %.c $(adresse_dir) $(socket_dir)
vpath %.h $(adresse_dir) $(socket_dir)
objects = tcp_socket.o adresse_internet.o server.o
executable = server

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)
	
server.o: server.c

socket_tcp.o: tcp_socket.c tcp_socket.h adresse_internet.h adresse_internet_type.h

adresse_internet.o: adresse_internet.c adresse_internet.h adresse_internet_type.h

