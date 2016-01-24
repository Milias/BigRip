CXXFLAGS += -std=c++14 -pedantic -Wall -g -Iheader -I../LuaBridge/Source/LuaBridge -Ijson  `sdl2-config --cflags`
LDFLAGS += -lSDL2_image -lSDL2_ttf -llua `sdl2-config --libs`
TARGET = bin/bigrip

CPP = $(wildcard *.cpp)
OBJ = $(CPP:.cpp=.o)

all : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $(CPP) $(LDFLAGS)

.PHONY = clean
clean :
	rm -f $(TARGET)
	rm -f *.o
