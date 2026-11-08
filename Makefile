CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = bin/jolly_jip_fis
SRC = src/main.c

all: $(TARGET)

$(TARGET):$(SRC)
	@mkdir -p bin
	$(CC)$(CFLAGS) $(SRC) -o$(TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf bin
