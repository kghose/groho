Notes on C++
============

Building
--------
- [Creating your own Cmake find module script for a library](https://cmake.org/Wiki/CMake:How_To_Find_Libraries)


Template class derived from a template class
--------------------------------------------
See `timestamps.hpp`. This derives from `std::vector`. The form of template is

```
template<typename T>
class Timestamps : public std::vector<T>
{  
public:
  size_t
  find( float jd )
  {
    if( std::vector<T>::size()  ==  0 )  return 0;
    if( this->size()  ==  0 )  return 0;
   
    ...
  }
};
```
Note how when we need to access the `size` member function we need to explicitly
call the base class function `size` or use `this`. This is in contrast to if 
we inherited `Timestamps` from, say, `std::vector<float>` directly in which case
we would not need to do this. The reason for this is fascinating and is given
very well [here](https://stackoverflow.com/a/4643295/2512851):

> Short answer: in order to make size() a dependent name, so that lookup is 
deferred until the template parameter is known.


Template classes must be defined in the header file
--------------------------------------------------
This was mildly annoying to find out. https://www.codeproject.com/Articles/48575/How-to-define-a-template-class-in-a-h-file-and-imp




Multi-threading
---------------

### Threads with class method use `std:ref`

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


###Condition variables

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


OpenGL specific
===============
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

Mapping: i.e. writing directly into the display buffer
------------------------------------------------------
From the OpenGL [wiki](https://www.khronos.org/opengl/wiki/Buffer_Object#Mapping):

> glBufferSubData is a nice way to present data to a buffer object. But it can be wasteful in performance, depending on your use patterns.
>
> For example, **if you have an algorithm that generates data that you want to store in the buffer object, you must first allocate some temporary memory to store that data in. Then you can use glBufferSubData to transfer it to OpenGL's memory. Similarly, if you want to read data back, glGetBufferSubData is perhaps not what you need, though this is less likely. It would be really nice if you could just get a pointer to the buffer object's storage and write directly to it.**
>
> You can. To do this, you must map the buffer. This gives you a pointer to memory that you can write to or read from, theoretically, just like any other. When you unmap the buffer, this invalidates the pointer (don't use it again), and the buffer object will be updated with the changes you made to it.

NICE!!! This is exactly what I'm looking for. The rest of the wiki tells us how to do this



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
**For FLTK, use the void* pointer argument**
Create a static member function and use the `void*` pointer to pass 
the `this` pointer through:
```
static void refresh_simulation_data( void* ptr )
{
  (Display*) ptr->whatever();
  Fl::repeat_timeout( config::sim_poll_interval, 
                      Display::refresh_simulation_data, ptr );  
}
```

And insert is as:
```
{
  ...
  Fl::add_timeout( config::sim_poll_interval, 
                   Display::refresh_simulation_data, this );
  // Poll simulation for new data periodically
}
```

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

