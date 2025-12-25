CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
SRC     = src/main.c src/utils.c
OBJ     = $(SRC:.c=.o)
TARGET  = list
LDFLAGS = -lncurses

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c include/utils.h
		$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
		cp $(TARGET) /usr/local/bin/

uninstall:
		rm -f /usr/local/bin/$(TARGET)

clean:
		rm -f $(OBJ) $(TARGET)
