cls
cl *.cpp glfw3dll.lib glew32.lib OpenGL32.lib /EHsc /W3 /O2 /MT /I./include /link /LIBPATH:./lib /LIBPATH:"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\x64" /out:meshing.exe
