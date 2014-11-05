hello-modern-opengl
===================

A one-file hello world program for modern OpenGL using GLFW (version 3) and GLEW.

Illustrates the most salient points of modern (3.3) OpenGL: Shaders, VAOs, packed VBOs, indexed elements. Contains all of the matrix math necessary for a look-at style camera.

This repository also contains an ES branch, which illustrates a similar program using OpenGL ES. While GLFW is used for this branch (take care in compiling it for ES and probably EGL, e.g.: `cmake -DGLFW_USE_EGL=ON -DGLFW_CLIENT_LIBRARY=glesv2`), GLEW is omitted because it does not support GL ES.
