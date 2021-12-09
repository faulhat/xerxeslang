CC=gcc
CFLAGS=-g

build: file_open.c macros.c main.c pointers.c proto_2.c
	$(CC) -o bin/xerxes file_open.c macros.c main.c pointers.c proto_2.c