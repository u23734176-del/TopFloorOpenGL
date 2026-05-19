# TopFloorOpenGL
This is the groups' Resopitory for the Homework Assignment. 

TopFloor-OpenGL/
├── src/
│   ├── main.cpp //main file to render
│   ├── core/           #  Genral Vertext Shaders, Genral  Fragementshaders
|   |── GolfCourse/     # The feature that holds layout 
|   |── Ligthing/ 
│   ├── layout/         # Course, skybox, terrain, water
│   ├── objects/        # Holes (1-18), windmill, gazebo, trees, etc.
│   ├── physics/        # Ball movement, collision detection
│   └── ui/             # HUD overlay
├── shaders/
│   ├── blinn_phong.vert/.frag
│   ├── shadow_map.vert/.frag
│   ├── water.vert/.frag
│   └── post_process.frag     # Camera filters
├── textures/           # All custom textures (grass, rock, concrete, water, dirt)
├── SkyBox/             # For the cube map images
├── include/            # Header files
├── docs/               # Report, annotated maps, reference images
├── CMakeLists.txt
└── README.md


Each team member clones, runs git checkout -b feature