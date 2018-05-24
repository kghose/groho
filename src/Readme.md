Code organization
=================

* buffer - code to handle data storage and sharing
* external - external libraries, currently just loguru
* magnumapp - Visualization window based on Magnum/Corrade
* orrery - Orrery models (NASA/JPL SPK files) loading and computation
* scenario - Scenario and flightplan loading and parsing


C++ Language features
=====================
I learned several things about C++ coding while doing this project. My haphazard
notes are here.


Code formatting
---------------
I was first resistant to but now am a zealous convert to `clang-format`. I just
have it trigger on file save and don't worry about formatting any more. How
LaTeX-like! The only thing I ocassionally fiddle with is leaving gaps between
variable declarations if I think the grouping leads to awkward formatting. My
`.clang-format` is based off WebKit. And there is always `// clang-format off`
for when I need it.


Mutable
-------
My strategy to pass data from the Simulator to the Display was to share a data
buffer between the two. The Buffer object contains a lock. I wanted to make sure
that a display (reader/consumer) could never alter the Buffer - it was the solely
the simulator's job to write to it. I was passing the Buffer as a `const` (pointer)
to the Display, but because of the mutex lock the compiler would complain. This
is fixed by `mutable` which allows us to mark out particular variables as
exceptions to the `const` requirement.


Optional
--------
`std::optional` is a C++17 feature that I was eager to use. It works fine in
practice. There are some creature comforts the compiler affords: inside the
function you don't have to operate on `std::optional<Foo>` you can just work
with `Foo` and `return Foo` and the compiler takes care of converting the
type. Also, when you want to return a `None` object, you can simply do
`return {}`. Examples are found in code used to load data, such as under 
`orrery` or `scanario`


Switch-board (Pythonic C++)
------------ 
Python doesn't have a switch statement while C++ does. One can create quite
an elegant switch statement by setting up a dictionary whose keys are the
options and values are lambda functions that perform apropriate actions.

In "modern" C++ thanks to `std::map`, initializer lists and lambda functions
one can use an identical paradigm with identical elegance. This came in very 
handy in scenario file parsing - I used this construct extensively
to interpret key value pairs and verb/noun lines.

```
typedef std::string sst;

bool set_key_value(std::optional<KeyValue> kv)
{
    std::unordered_map<sst, std::function<void(sst)>> keyword_map{

        { "name", [=](sst v) { name                     = v; } },
        { "max-acceleration", [=](sst v) { max_acc      = stof(v); } },
        { "max-fuel", [=](sst v) { max_fuel             = stof(v); } },
        { "fuel-cons-rate", [=](sst v) { fuel_cons_rate = stof(v); } },
        { "flight-state", [=](sst v) { flight_state     = FALLING; } },
        // TODO: handle landed state
        { "position", [=](sst v) { position             = stoV(v); } },
        { "velocity", [=](sst v) { velocity             = stoV(v); } }

    };

    if (keyword_map.count(kv->key)) {
        keyword_map[kv->key](kv->value);
        return true;
    } else {
        return false;
    }
};
```

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


Units (user defined literals)
-----------------------------





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

Fonts, FLTK and OpenGL3
-----------------------

FLTK uses ftgl


http://jonmacey.blogspot.com/2011/10/text-rendering-using-opengl-32.html

http://devcry.heiho.net/html/2012/20120115-opengl-font-rendering.html

https://github.com/wjakob/nanogui


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


Computation strategies
======================

Sharing a data buffer between writer and reader
-----------------------------------------------
In my application I have a producer (A Simulator) that continuously generates
data. A consumer (the Display) would, ideally, like to reflect the current
state of the data without hindering the producer.

The simplest solution is to lock the data structure, say an array, when writing
or reading. In my application this works fine because the Display only needs to
read occasionally, so there is not much contention.

For a more intensive application (reader reads very often) we can segment the
data into blocks. The writer works on a block, closes it and then moves onto
a new block. The reader can read any block except the one the writer is 
currently working on.

In fact, if we ensure that the current block is never reallocated e.g. we 
use a vector and preallocate the block, we can then let the reader read upto
the current data point (that the writer is working on). This code is more 
involved, however.