# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall

# The build target executable
TARGET = oss

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)

