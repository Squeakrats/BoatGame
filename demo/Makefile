CXX=clang++
FLAGS=-std=c++11
INC_PATHS=-I../source -I../libs/include
LIB_PATHS=-L../libs/lib 
LIBS=-lGLEW -lGLU -lGL -lXrandr -lXi -lXrender -ldrm -lXdamage -lXxf86vm -lXext -lX11 -lpthread  -lGLEW -lglfw3 -lassimp
SOURCES=$(wildcard ../source/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

all : client server nettest


client : $(OBJECTS) client.cpp
	$(CXX) $(FLAGS) $(OBJECTS) client.cpp -o ../bin/client $(INC_PATHS) $(LIB_PATHS) $(LIBS) 

server : $(OBJECTS) server.cpp
	$(CXX) $(FLAGS) $(OBJECTS) server.cpp -o ../bin/server $(INC_PATHS) $(LIB_PATHS) $(LIBS) 

nettest : $(OBJECTS) nettest.cpp
	$(CXX) $(FLAGS) $(OBJECTS) nettest.cpp -o ../bin/nettest $(INC_PATHS) $(LIB_PATHS) $(LIBS) 

../source/%.o : ../source/%.cpp ../source/%.h
	$(CXX) $(FLAGS) -c $< -o $@ $(INC_PATHS)

