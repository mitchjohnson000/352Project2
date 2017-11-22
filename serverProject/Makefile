TARGET = serverrunnable
LIBS = -lm -lpthread -lcrypto
CC = gcc
CFLAGS = -g -w

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = server.o
HEADERS = 

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -w $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
