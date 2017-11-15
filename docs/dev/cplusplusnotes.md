Notes on C++
============

Building
========
- [Creating your own Cmake find module script for a library](https://cmake.org/Wiki/CMake:How_To_Find_Libraries)


Multi-threading
===============

Threads with class method use `std:ref`
-------------------------------------
When passing a class method as the function to run in a thread use the following
syntax (used in `main.cpp`, for example):

```

  class Simulation
  {
    ...
    void loop() {;}
  }


  sim::Simulation simulation( scenario_file );

  ...

  std::thread simulation_thread( 
    &sim::Simulation::loop,
    std::ref( simulation )
  );
```
Thread wants to make a copy of the object it's being passed, so if you would
rather that the live version of the object be used, you should pass a reference
using std::ref


Condition variables
------------------

- issue with atomic

http://en.cppreference.com/w/cpp/thread/condition_variable


Sharing a data structure
------------------------
Our use case is that we have a data structure that is written to from one
thread and read from another. The data structure is a container for a series
of pointers to our data. We'd like to not freeze the entire data structure
at any time - so we'd like to be able to read some data while writing other
data to avoid slowing down our code unecessarily.

We do this by having thread safe accessors that lock just the part of the data
we want to read/write


OpenGL
======
After a lot of thrashing around and getting bits and pieces, I found Tom Dalling's
[Modern OpenGL tutorial][dalling], which I find has the right pace and level of
description. One issue with the code is that he hides some of the opengl code
behind some abstractions which makes it a little hard to figure out what the
opengl calls actually are.

[dalling]: https://www.tomdalling.com/blog/modern-opengl/01-getting-started-in-xcode-and-visual-cpp/

There is the very popular [OpenGL tutorial][ogl] which is also good, and I refer
to this also but I found the ordering of Dalling's tutorial better.

[ogl]: http://www.opengl-tutorial.org/


FLTK and OpenGL 3 on mac
------------------------
  - http://www.fltk.org/doc-1.3/opengl.html  ("Using OpenGL 3.0 (or higher versions)")
  - https://github.com/IngwiePhoenix/FLTK/blob/master/examples/OpenGL3-glut-test.cxx
  - In order for resizing to work properly (and on the retina display) I needed
    - to set `size_range`
    - to use `pixel_h()` and `pixel_w()`

Misc
----
- Many GLFW functions can only run in the [main thread](http://www.glfw.org/docs/latest/intro_guide.html#thread_safety)
- Picked FLTK because GLFW was extremely limited, though simple
  - FLTK gives us C++ windowing system
  - Can draw text
  - Gives us glu and glut!!
  - Nice tutorials exist
    - http://web.cecs.pdx.edu/~fliu/courses/cs447/tutorial6.html
    - https://solarianprogrammer.com/2013/05/13/opengl-101-drawing-primitives/


Passing member function pointers to callbacks
---------------------------------------------
**For GLFW Use WindowUserPointers and a wrapper**
From https://stackoverflow.com/a/28660673/2512851
```
  glfwSetWindowUserPointer( window, this );
  auto func = [](GLFWwindow* w, int, int, int)
  {
      static_cast<Display*>(glfwGetWindowUserPointer(w))->mouseButtonPressed( /* ... */ );
  };
  glfwSetMouseButtonCallback( window, func );
```

If you use a frame work without this, you effectively need to have a globally
accessible pointer to your object.

The use of "f" in code using OpenGL
-----------------------------------
This annoyed the heck out of me until I ran into a compile error and read [this](https://www.opengl.org/discussion_boards/showthread.php/175108-Scaling-a-vec3?p=1224244&viewfull=1#post1224244). It explains that 2.0 is interpreted as a double 

which explains that C++ templates are strict and since
the underlying functions operate of floats, passing a number like 2.0 

