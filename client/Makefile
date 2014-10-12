CXX=clang++
FLAGS=-std=c++11
LIBRARIES=
LIBRARY_PATH=-L../libs/lib 
INCLUDE_PATHS=-I../source/ -I../libs/include/

SOURCES=$(wildcard ../source/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

all : client 

client : ../source/UDPSocket.cpp main.cpp
	$(CXX) $(FLAGS) ../source/UDPSocket.cpp main.cpp -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATH) $(LIBRARIES)

../source/%.o : ../source/%.cpp
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATH) $(LIBRARIES)
