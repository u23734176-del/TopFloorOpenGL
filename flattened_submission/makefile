# Generic Makefile for flat directory
CXX = g++
CXXFLAGS = -std=c++11 -g -Wall
LDFLAGS = -lglfw -lGLEW -lGL -lassimp -pthread -ldl

# Find all .cpp and .c files in the current directory
SRCS = $(wildcard *.cpp) $(wildcard *.c)
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

run: all
	./$(TARGET)