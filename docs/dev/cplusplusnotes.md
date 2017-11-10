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

Misc
----
- Many GLFW functions can only run in the [main thread](http://www.glfw.org/docs/latest/intro_guide.html#thread_safety)
- Picked FLTK because GLFW was extremely limited, though simple
  - FLTK gives us C++ windowing system
  - Can draw text
  - Gives us glu and glut!!
  - Nice tutorial exists (http://web.cecs.pdx.edu/~fliu/courses/cs447/tutorial6.html)


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