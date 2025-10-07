# CXX = g++
# CXXFLAGS = -Wall -std=c++11
# LIBS = -lglfw -lglew32 -lopengl32 -lgdi32

# all: red_triangle blue_square shapes creative

# red_triangle: red_triangle.cpp
# 	$(CXX) $(CXXFLAGS) -o red_triangle red_triangle.cpp $(LIBS)

# blue_square: blue_square.cpp
# 	$(CXX) $(CXXFLAGS) -o blue_square blue_square.cpp $(LIBS)

# shapes: shapes.cpp
# 	$(CXX) $(CXXFLAGS) -o shapes shapes.cpp $(LIBS)

# creative: creative.cpp
# 	$(CXX) $(CXXFLAGS) -o creative creative.cpp $(LIBS)

# clean:
# 	rm -f red_triangle.exe blue_square.exe shapes.exe creative.exe

Compiler
CXX = g++

Compiler flags:
-std=c++11 for standard compliance
-Wall for all warnings
-g for debugging information
CXXFLAGS = -std=c++11 -Wall -g

Libraries to link:
-lGL for OpenGL
-lglut (or -lfreeglut) for window management
-lGLEW or -lglad for loading modern OpenGL functions
-lm for math functions (like sin/cos)
LDFLAGS = -lGL -lglut -lglad -lm

Executable name
TARGET = a2_interactive_shapes

Source files
SOURCES = a2_interactive_shapes.cpp shader.vert shader.frag
OBJECTS = $(filter %.o, $(SOURCES:.cpp=.o))

Default target: compile and link everything
.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
@echo "Linking target: $@"
$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

Rule for compiling C++ files
%.o: %.cpp
@echo "Compiling $<"
$(CXX) $(CXXFLAGS) -c $< -o $@

Rule to clean up generated files
.PHONY: clean
clean:
@echo "Cleaning up..."
rm -f $(TARGET) *.o
