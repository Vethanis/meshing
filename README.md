# meshing
An interactive application for sculpting in 3D. 

Distance functions defining the surface of a shape can be added additively and subtractively to form complex shapes.
Functions are added to an octree which causes affected areas to be re-evaluated.
Function evaluation consists of iterating over a hierarchical grid to find distances smaller than some threshold.
Zero-crossing points are collected and uploaded to an openGL vertex buffer, then drawn.

## **Video**

[![](http://img.youtube.com/vi/PMKJSMjiwCs/0.jpg)](http://www.youtube.com/watch?v=PMKJSMjiwCs)

## __Controls:__

| **Input**      | **Action**           |
|----------------|----------------------|
| Mouse movement | Look around          |
| Left click     | Add brush            |
| Right click    | Remove brush         |
| 1              | Toggle shape         |
| Up arrow       | Increase brush size  |
| Down arrow     | Descrease brush size |
| Left arrow     | Decrease blending    |
| Right arrow    | Increase blending    |
| W              | Move forward         |
| S              | Move backward        |
| A              | Move left            |
| D              | Move right           |
| Left shift     | Move down            |
| Spacebar       | Move up              |
| E              | Set light direction  |
| NUM 7          | Increase red         |
| NUM 4          | Decrease red         |
| NUM 8          | Increase green       |
| NUM 5          | Decrease green       |
| NUM 9          | Increase blue        |
| NUM 6          | Decrease blue        |
| ESC            | Close program        |

## __Dependencies:__

* OpenGL 3.3
* glew
* glfw3
* glm
* c++11 compiler
* cmake
  
## __Building:__

* mkdir build
* cd build
* cmake .. -G "*your platform*"
* cd ..
* cmake --build build --config Release

## __Running:__

* cd bin
* ./main <width> <height> 
