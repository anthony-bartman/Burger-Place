CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS= -pthread
SOURCES=main.c burger.c ll.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@
.c.o:
		$(CC) $(CFLAGS) $< -o $@
clean:
		rm -rf $(OBJECTS) $(EXECUTABLE)
