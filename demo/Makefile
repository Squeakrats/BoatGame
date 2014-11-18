CXX=clang++
FLAGS=-std=c++11
INC_PATHS=-I../source -I../libs/include
LIB_PATHS=-L../libs/lib 
LIBS=-lGLEW -lGLU -lGL -lXrandr -lXi -lXrender -ldrm -lXdamage -lXxf86vm -lXext -lX11 -lpthread  -lGLEW -lglfw3 -lassimp -lILUT -lILU -lIL -lfreeimage
SOURCES=$(wildcard ../source/*.cpp)
HEADERS=$(SOURCES:.cpp=.h)
OBJECTS=$(SOURCES:.cpp=.o)
DEPS=$(SOURCES:.cpp=.d)
-include $(DEP)
all : client server nettest


client : $(OBJECTS) client.cpp
	$(CXX) $(FLAGS) $(OBJECTS) client.cpp -MD -MF $@.d -o ../bin/client $(INC_PATHS) $(LIB_PATHS) $(LIBS) 

server : ../source/Actor.o ../source/SideViewActor.o ../source/UDPSocket.o server.cpp
	$(CXX) $(FLAGS) ../source/Actor.o ../source/SideViewActor.o ../source/UDPSocket.o server.cpp -o ../bin/server $(INC_PATHS)

nettest : $(OBJECTS) nettest.cpp
	$(CXX) $(FLAGS) $(OBJECTS) nettest.cpp -o ../bin/nettest $(INC_PATHS) $(LIB_PATHS) $(LIBS) 


../source/%.o : ../source/%.cpp $(HEADERS)
	$(CXX) $(FLAGS) -c $< -o $@ $(INC_PATHS)

