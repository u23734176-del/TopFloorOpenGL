# TopFloorOpenGL
This is the groups' Resopitory for the Homework Assignment.Click on the edit file to see better (don't attually edit now till we do work)


```
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
```


You need a .gitkeep file to track empty folders in Git. By default, Git only tracks files and completely ignores empty directories. If you need a specific folder structure for your app to work (like an empty /logs or /uploads directory), .gitkeep acts as a placeholder to trick Git into committing and pushing that folder.


Each team member clones, runs git checkout -b feature
