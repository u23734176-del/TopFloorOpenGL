#!/bin/bash
set -e

echo "1. Creating submission staging directory..."
mkdir -p submission_build

echo "2. Flattening source files..."
find src -type f -name "*.cpp" -exec cp {} submission_build/ \;
find src -type f -name "*.h" -exec cp {} submission_build/ \;

echo "3. Rewriting include paths to flat structure..."
# Replaces #include "folder/File.h" with #include "File.h"
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS sed
    find submission_build -type f \( -name "*.cpp" -o -name "*.h" \) -exec sed -i '' -E 's/#include +"[a-zA-Z0-9_]+\/([^"]+)"/#include "\1"/g' {} +
else
    # Linux sed
    find submission_build -type f \( -name "*.cpp" -o -name "*.h" \) -exec sed -i -E 's/#include +"[a-zA-Z0-9_]+\/([^"]+)"/#include "\1"/g' {} +
fi

echo "4. Copying assets..."
cp -r shaders/ submission_build/shaders/
cp -r textures/ submission_build/textures/
cp -r skybox/ submission_build/skybox/

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

run: all
	./$(TARGET)

.PHONY: all clean run
EOF

echo "6. Executing Clean Clone Build Test..."
cd submission_build
make clean
make

echo "Build successful. Zipping archive..."
cd ..
zip -r TopFloor_MiniGolf_Archive.zip submission_build/
echo "Done. Upload TopFloor_MiniGolf_Archive.zip to ClickUp and FitchFork."