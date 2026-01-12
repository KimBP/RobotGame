# Makefile for Edge Patrol Robot
CXX = g++
CXXFLAGS = -std=c++11 -fPIC -I../../
LDFLAGS = -shared -ldl

# Source files
SRC = EdgePatrol.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = EdgePatrol.so

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
