CXX = g++
# GLAD include files are inside the GLFW includes folder
CXXFLAGS = -Wall -Wextra -g -I"C:\Program Files (x86)\GLFW\include"
LDFLAGS = -L"C:\Program Files (x86)\GLFW\lib" -lglfw3 -lopengl32 -lgdi32

SRC = FluidSim.cpp glad.c
OBJ = $(SRC:.cpp=.o)
OBJ := $(OBJ:.c=.o)

TARGET = FluidSim

all:	$(TARGET)

$(TARGET):	$(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


# Rules to compile files only if changed
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	del -f $(TARGET).exe *.o