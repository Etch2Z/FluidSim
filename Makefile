CXX = g++
# GLAD include files are inside the GLFW includes folder

# C:\Program Files (x86)\GLFW\lib
# C:\Users\zengh\OneDrive\Desktop\FluidSim\GLFW_includes
# -Wextra
CXXFLAGS = -Wall  -g -I".\GLFW_includes\include"
LDFLAGS = -L".\GLFW_includes\lib" -lglfw3 -lopengl32 -lgdi32


SRC = main.cpp glad.c FluidSim.cpp

# Rule to make .o files out of SRC
OBJ = $(SRC:.cpp=.o)
OBJ := $(OBJ:.c=.o)

TARGET = main

OUT = FluidSim

all:	$(TARGET)

$(TARGET):	$(OBJ)
	$(CXX) $(CXXFLAGS) -o $(OUT) $^ $(LDFLAGS)

main.o: main.h shaders.h

# Rules to compile files only if changed
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	del -f $(OUT).exe *.o