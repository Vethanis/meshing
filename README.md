# meshing
An interactive application for sculpting in 3D. 

Distance functions defining the surface of a shape can be added additively and subtractively to form complex shapes.
Functions are added to an octree which causes affected areas to be re-evaluated.

__Controls:__
* mouse movement: look around
* left click: apply brush additively
* right click: apply brush subtractively
* 1 key: toggle between sphere and cube brushes
* Up Arrow, Down Arrow: increase or decrease brush size
* 3 and 4 keys: decrease or increase brush fidelity
* WS: forward and backward
* AD: left and right
* left shift, space: down and up

[Video](https://youtu.be/6Rybvj4Wzpk)

__Dependencies:__
* OpenGL 4.3
* glew
* glfw3
* glm
* clang
* make
  
__Building:__
* make -j release

__Running:__
* make run

![alt tag](http://i.imgur.com/fyDl3kW.png)
