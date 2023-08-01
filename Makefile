# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Libraries
LIBS = -lpcap -lz -lrdkafka

# Source files
SRCS = capture.c config.c call.c sox.c kafka.c binary.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = capture

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)