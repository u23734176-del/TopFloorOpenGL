#!/bin/bash
set -e

echo "1. Creating submission staging directory..."
mkdir -p submission_build

echo "2. Flattening source files..."
find src -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec cp {} submission_build/ \;

echo "3. Converting all includes to local style..."
# This regex looks for any #include "folder/file.ext" and turns it into #include "file.ext"
# It works regardless of whether the folder is there or not.
if [[ "$OSTYPE" == "darwin"* ]]; then
    find submission_build -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec sed -i '' -E 's/#include +"[^"]*\/([^"]+)"/#include "\1"/g' {} +
else
    find submission_build -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec sed -i -E 's/#include +"[^"]*\/([^"]+)"/#include "\1"/g' {} +
fi

echo "4. Copying assets (flattening asset directories)..."
mkdir -p submission_build/shaders submission_build/textures submission_build/skybox

# Use 'cp -r folder/* destination/' to copy contents, not the folder itself
cp -r shaders/* submission_build/shaders/ 2>/dev/null || :
cp -r textures/* submission_build/textures/ 2>/dev/null || :
cp -r skybox/* submission_build/skybox/ 2>/dev/null || :

echo "5. Generating flat Makefile..."
cat << 'EOF' > submission_build/Makefile
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = -lglfw -lGL -lGLEW -ldl

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = mini_golf

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean run
EOF

echo "6. Build Test..."
cd submission_build
make clean
make
echo "Packaging complete. Check submission_build/."