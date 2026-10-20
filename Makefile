CC = gcc

CFLAGS= -pedantic -Wvla -Wall -g -Wextra -Iinclude

LDFLAGS=-lm

SRCS = $(shell find src/ -name '*.c')

OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

TARGET= monoc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -g -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c 
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -g -c $< -o $@


clean:
	rm -rf build $(TARGET)

.PHONY: all clean monoc
