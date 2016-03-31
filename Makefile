CC=g++
CFLAGS=-c -Wall -std=c++11
SOURCES=main.cpp container.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=boxie

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *o $(EXECUTABLE)