CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = raw_socket_demo

.PHONY: all clean

all: $(TARGET)

$(TARGET): raw_socket_demo.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
