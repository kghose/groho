/*
  This is how we get OpenGL3 on FLTK in a multi-platform way
  
  http://www.fltk.org/doc-1.3/opengl.html  ("Using OpenGL 3.0 (or higher versions)")

  https://github.com/IngwiePhoenix/FLTK/blob/master/examples/OpenGL3-glut-test.cxx
*/
#pragma once

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif

#include <vector>


const std::vector<GLfloat> companion_cube = {  // For testing. Get it?
    //  X     Y     Z
    // bottom
    -1.0f,-1.0f,-1.0f,  
      1.0f,-1.0f,-1.0f, 
    -1.0f,-1.0f, 1.0f,  
      1.0f,-1.0f,-1.0f, 
      1.0f,-1.0f, 1.0f, 
    -1.0f,-1.0f, 1.0f,  

    // top
    -1.0f, 1.0f,-1.0f,  
    -1.0f, 1.0f, 1.0f,  
      1.0f, 1.0f,-1.0f, 
      1.0f, 1.0f,-1.0f, 
    -1.0f, 1.0f, 1.0f,  
      1.0f, 1.0f, 1.0f, 

    // front
    -1.0f,-1.0f, 1.0f,  
      1.0f,-1.0f, 1.0f, 
    -1.0f, 1.0f, 1.0f,  
      1.0f,-1.0f, 1.0f, 
      1.0f, 1.0f, 1.0f, 
    -1.0f, 1.0f, 1.0f,  

    // back
    -1.0f,-1.0f,-1.0f,  
    -1.0f, 1.0f,-1.0f,  
      1.0f,-1.0f,-1.0f, 
      1.0f,-1.0f,-1.0f, 
    -1.0f, 1.0f,-1.0f,  
      1.0f, 1.0f,-1.0f, 

    // left
    -1.0f,-1.0f, 1.0f, 
    -1.0f, 1.0f,-1.0f, 
    -1.0f,-1.0f,-1.0f, 
    -1.0f,-1.0f, 1.0f, 
    -1.0f, 1.0f, 1.0f, 
    -1.0f, 1.0f,-1.0f, 

    // right
    1.0f,-1.0f, 1.0f, 
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f, 
    1.0f,-1.0f, 1.0f, 
    1.0f, 1.0f,-1.0f, 
    1.0f, 1.0f, 1.0f, 
};