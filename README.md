# meshing
An interactive application for sculpting in 3D. 

Distance functions defining the surface of a shape can be added additively and subtractively to form complex shapes.
Functions are added to an octree which causes affected areas to be re-evaluated.
Function evaluation consists of iterating over a hierarchical grid to find distances smaller than some threshold.
Zero-crossing points are collected and uploaded to an openGL vertex buffer, then drawn.

__[Video](https://youtu.be/6Rybvj4Wzpk)__

__Controls:__
* mouse movement: look around
* left click: apply brush additively
* right click: apply brush subtractively
* 1 key: toggle between sphere and cube brushes
* Up Arrow, Down Arrow: increase or decrease brush size
* 3 and 4 keys: increase and decrease brush fidelity
* WS: forward and backward
* AD: left and right
* left shift, space: down and up

__Linux Dependencies:__
* OpenGL 4.3
* glew
* glfw3
* glm
* clang
* make
  
__Linux Building:__
* make -j release

__Linux Running:__
* make run

__Windows Usage:__
* requires VS2015 installed at default location, or an x64 cmd environment
* requires Windows 7.1A SDK installed in default location for OpenGL32.lib
* loadenv.bat
* build.bat
* run.bat

__[Windows Binary](https://drive.google.com/open?id=0B3fK-D-3t1mGaWJsZ0xMb0JCUFk)__

![alt tag](http://i.imgur.com/fyDl3kW.png)
