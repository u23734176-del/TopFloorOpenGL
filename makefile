#add all the files that you need to the files makefile variable below as a space seperated list
files = \
src/core/Camera.cpp \
src/core/Scene.cpp \
src/core/ShaderManager.cpp \
src/objects/Cube.cpp \
src/core/shader.cpp

main: src/main.cpp src/glad.c
	g++ -std=c++11 -g $(files) src/main.cpp src/glad.c -lglfw3 -pthread -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run:
	./main

all:
	make clean
	make
	make run