# Enter 'make' to build files
# Enter 'make clean' to remove build files
# Run './appserver' after build for program

TARGET = appserver
LIBS = -lm -lpthread
CC = gcc
CFLAGS = -g -Wall

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
