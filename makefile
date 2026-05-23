#add all the files that you need to the files makefile variable below as a space seperated list
files = \
src/core/Camera.cpp \
src/core/Scene.cpp \
src/core/ShaderManager.cpp \
src/core/shader.cpp \
src/core/SceneObject.cpp \
src/core/ResourceManager.cpp \
src/objects/Cube.cpp \
stb_image.cpp \
src/lighting/shadowMap.cpp \
src/ui/HUD.cpp \
src/ui/PostProcessor.cpp \
src/objects/Cylinder.cpp \
src/objects/Cone.cpp \
src/objects/Rock.cpp \
src/objects/Tunnel.cpp \
src/objects/GolfHole.cpp \
src/layout/Course.cpp \
src/objects/Windmill.cpp \
src/physics/Collision.cpp \
src/physics/PhysicsWorld.cpp \
src/physics/BallPhysics.cpp \
# src/objects/SphereBall.cpp \
# src/objects/Decoration.cpp \

main: src/main.cpp src/glad.c
	g++ -std=c++11 -g $(files) src/main.cpp src/glad.c -lglfw -pthread -lGLEW -ldl -lGL -o main

clean:
	rm -f *.o main

run:
	./main

all:
	make clean
	make
	make run

# made changes