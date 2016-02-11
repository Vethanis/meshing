EXE = Renderer.exe

ifeq ($(OS), Windows_NT)
	CXX = g++
	INC_DIRS = -I./include
	CXXFLAGS = $(INC_DIRS) -std=c++11 -Wfatal-errors -Wall
	LDFLAGS = -L./lib
	LDLIBS = .\glfw3.dll -lglew32 -lopengl32
else 
	ifeq ($(shell uname), Linux)
		CXX = clang++
		INC_DIRS = -I/usr/include
		CXXFLAGS = $(INC_DIRS) -std=c++11 -pthread -stdlib=libstdc++ -Wfatal-errors -Wall
		LDFLAGS = -L/usr/lib
		LDLIBS = -lGLEW -lGL -lglfw -pthread
	endif
endif


LINK = $(CXX) -o
COMPILE = $(CXX) -c
 
CPP_FILES = $(wildcard *.cpp)
OBJ_FILES = $(CPP_FILES:.cpp=.o)
 
.PHONY: clean all run debug release
 
all: $(EXE)
 
debug:	CXXFLAGS += -DDEBUG -g 
debug:	$(EXE)

release: CXXFLAGS += -O3
release: $(EXE)

run:	$(EXE)
	./$(EXE) 1280 720

clean:
	rm *.o $(EXE)

$(EXE):		$(OBJ_FILES)
	$(LINK) $(EXE) $(OBJ_FILES) $(LDFLAGS) $(LDLIBS) 
	
main.o: 	main.cpp
	$(COMPILE) $< $(CXXFLAGS)

%.o: 	%.cpp %.h
	$(COMPILE) $< $(CXXFLAGS)
