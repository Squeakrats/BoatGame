CXX=clang++
FLAGS=-std=c++11
LIBRARIES=-lGLEW -lGLU -lGL -lXrandr -lXi -lXrender -ldrm -lXdamage -lXxf86vm -lXext -lX11 -lpthread  -lGLEW -lglfw3 -lassimp
LIBRARY_PATH=-L../libs/lib 
INCLUDE_PATHS=-I../source/ -I../libs/include/

SOURCES=../source/Actor.cpp ../source/SideViewActor.cpp ../source/UDPSocket.cpp
OBJECTS=$(SOURCES:.cpp=.o)

all : server 

server : $(OBJECTS) main.cpp
	$(CXX) $(FLAGS) $(OBJECTS) main.cpp -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATH) $(LIBRARIES)

../source/%.o : ../source/%.cpp
	$(CXX) $(FLAGS) -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATH) $(LIBRARIES)
