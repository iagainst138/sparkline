CC=gcc
CFLAGS=-lm -Wall
TARGET=sparkline

$(TARGET): sparkline.c
	$(CC) -o $(TARGET) sparkline.c $(CFLAGS)

clean:
	rm $(TARGET)
