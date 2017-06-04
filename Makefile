CC=gcc
OUTPUT=test_ezjson

all:
	$(CC) test_ezjson.c ezjson.c -o $(OUTPUT)
