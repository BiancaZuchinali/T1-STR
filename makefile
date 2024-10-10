TARGET = simulator

SOURCE = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)

CC = g++

all: $(TARGET)

$(TARGET): $(SOURCE) $(HEADERS)
	$(CC) -o $(TARGET) $(SOURCE)