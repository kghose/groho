Developer notes
===============

<!-- TOC -->

- [Code organization](#code-organization)
- [C++ Language features](#c-language-features)
    - [Code formatting](#code-formatting)
    - [Mutable](#mutable)
    - [Optional](#optional)
    - [Switch-board (Pythonic C++)](#switch-board-pythonic-c)
    - [Building](#building)
    - [Template class derived from a template class](#template-class-derived-from-a-template-class)
    - [Template classes must be defined in the header file](#template-classes-must-be-defined-in-the-header-file)
    - [Units (user defined literals)](#units-user-defined-literals)
    - [Multi-threading](#multi-threading)
        - [Threads with class method use `std:ref`](#threads-with-class-method-use-stdref)
        - [Condition variables](#condition-variables)
- [OpenGL specific](#opengl-specific)
    - [FLTK and OpenGL 3 on mac](#fltk-and-opengl-3-on-mac)
    - [Mapping: i.e. writing directly into the display buffer](#mapping-ie-writing-directly-into-the-display-buffer)
    - [Fonts, FLTK and OpenGL3](#fonts-fltk-and-opengl3)
    - [Misc](#misc)
    - [Passing member function pointers to callbacks](#passing-member-function-pointers-to-callbacks)
    - [The use of "f" in code using OpenGL](#the-use-of-f-in-code-using-opengl)
- [Code design considerations](#code-design-considerations)
    - [Sharing a data buffer between writer and reader](#sharing-a-data-buffer-between-writer-and-reader)
    - [Fractal downsampler](#fractal-downsampler)
        - [Long straight trajectories](#long-straight-trajectories)
    - [How much and what data to store](#how-much-and-what-data-to-store)
        - [Sampling rate](#sampling-rate)
        - [Re-framing data](#re-framing-data)
        - [The decision](#the-decision)
        - [YAGNI](#yagni)
    - [Simulation restarts and checkpoints](#simulation-restarts-and-checkpoints)
    - [Simulation](#simulation)
- [Interface design considerations](#interface-design-considerations)
    - [Scaling data for display](#scaling-data-for-display)
    - [Thinking aloud on user interfaces](#thinking-aloud-on-user-interfaces)
        - [The decision](#the-decision-1)
    - [How would annotations work?](#how-would-annotations-work)
        - [Annotation script](#annotation-script)

<!-- /TOC -->

# Code organization

* buffer - code to handle data storage and sharing
* external - external libraries, currently just loguru
* magnumapp - Visualization window based on Magnum/Corrade
* orrery - Orrery models (NASA/JPL SPK files) loading and computation
* scenario - Scenario and flightplan loading and parsing


# C++ Language features

I learned several things about C++ coding while doing this project. My haphazard
notes are here.


## Code formatting

I was first resistant to but now am a zealous convert to `clang-format`. I just
have it trigger on file save and don't worry about formatting any more. How
LaTeX-like! The only thing I ocassionally fiddle with is leaving gaps between
variable declarations if I think the grouping leads to awkward formatting. My
`.clang-format` is based off WebKit. And there is always `// clang-format off`
for when I need it.


## Mutable

My strategy to pass data from the Simulator to the Display was to share a data
buffer between the two. The Buffer object contains a lock. I wanted to make sure
that a display (reader/consumer) could never alter the Buffer - it was the solely
the simulator's job to write to it. I was passing the Buffer as a `const` (pointer)
to the Display, but because of the mutex lock the compiler would complain. This
is fixed by `mutable` which allows us to mark out particular variables as
exceptions to the `const` requirement.


## Optional

`std::optional` is a C++17 feature that I was eager to use. It works fine in
practice. There are some creature comforts the compiler affords: inside the
function you don't have to operate on `std::optional<Foo>` you can just work
with `Foo` and `return Foo` and the compiler takes care of converting the
type. Also, when you want to return a `None` object, you can simply do
`return {}`. Examples are found in code used to load data, such as under 
`orrery` or `scanario`


## Switch-board (Pythonic C++)

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

## Building

- [Creating your own Cmake find module script for a library](https://cmake.org/Wiki/CMake:How_To_Find_Libraries)


## Template class derived from a template class

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


## Template classes must be defined in the header file

This was mildly annoying to find out. https://www.codeproject.com/Articles/48575/How-to-define-a-template-class-in-a-h-file-and-imp


## Units (user defined literals)

## Multi-threading

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


# OpenGL specific

After a lot of thrashing around and getting bits and pieces, I found Tom Dalling's
[Modern OpenGL tutorial][dalling], which I find has the right pace and level of
description. One issue with the code is that he hides some of the opengl code
behind some abstractions which makes it a little hard to figure out what the
opengl calls actually are.

[dalling]: https://www.tomdalling.com/blog/modern-opengl/01-getting-started-in-xcode-and-visual-cpp/

There is the very popular [OpenGL tutorial][ogl] which is also good, and I refer
to this also but I found the ordering of Dalling's tutorial better.

[ogl]: http://www.opengl-tutorial.org/


## FLTK and OpenGL 3 on mac

  - http://www.fltk.org/doc-1.3/opengl.html  ("Using OpenGL 3.0 (or higher versions)")
  - https://github.com/IngwiePhoenix/FLTK/blob/master/examples/OpenGL3-glut-test.cxx
  - In order for resizing to work properly (and on the retina display) I needed
    - to set `size_range`
    - to use `pixel_h()` and `pixel_w()`

## Mapping: i.e. writing directly into the display buffer

From the OpenGL [wiki](https://www.khronos.org/opengl/wiki/Buffer_Object#Mapping):

> glBufferSubData is a nice way to present data to a buffer object. But it can be wasteful in performance, depending on your use patterns.
>
> For example, **if you have an algorithm that generates data that you want to store in the buffer object, you must first allocate some temporary memory to store that data in. Then you can use glBufferSubData to transfer it to OpenGL's memory. Similarly, if you want to read data back, glGetBufferSubData is perhaps not what you need, though this is less likely. It would be really nice if you could just get a pointer to the buffer object's storage and write directly to it.**
>
> You can. To do this, you must map the buffer. This gives you a pointer to memory that you can write to or read from, theoretically, just like any other. When you unmap the buffer, this invalidates the pointer (don't use it again), and the buffer object will be updated with the changes you made to it.

NICE!!! This is exactly what I'm looking for. The rest of the wiki tells us how to do this

## Fonts, FLTK and OpenGL3


FLTK uses ftgl


http://jonmacey.blogspot.com/2011/10/text-rendering-using-opengl-32.html

http://devcry.heiho.net/html/2012/20120115-opengl-font-rendering.html

https://github.com/wjakob/nanogui


## Misc

- Many GLFW functions can only run in the [main thread](http://www.glfw.org/docs/latest/intro_guide.html#thread_safety)
- Picked FLTK because GLFW was extremely limited, though simple
  - FLTK gives us C++ windowing system
  - Can draw text
  - Gives us glu and glut!!
  - Nice tutorials exist
    - http://web.cecs.pdx.edu/~fliu/courses/cs447/tutorial6.html
    - https://solarianprogrammer.com/2013/05/13/opengl-101-drawing-primitives/


## Passing member function pointers to callbacks

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



## The use of "f" in code using OpenGL

This annoyed the heck out of me until I ran into a compile error and read [this](https://www.opengl.org/discussion_boards/showthread.php/175108-Scaling-a-vec3?p=1224244&viewfull=1#post1224244). It explains that 2.0 is interpreted as a double 

which explains that C++ templates are strict and since
the underlying functions operate of floats, passing a number like 2.0 


# Code design considerations 


## Sharing a data buffer between writer and reader

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


## Fractal downsampler

The exciting story behind the fractal downsampler is in this [IPython notebook][downsampler].

### Long straight trajectories

The downsampler works great and gives us all the benefits we hoped for, but
there are some aesthetic and scientific issues related to straight trajectories.
A spacecraft flying with high constant acceleration has a very straight 
trajectory and the display will be jerky. 

The solution chosen is to force a flush of the buffer when the display 
asks to see it. Whether we have anything to flush depends on whether we've
accumulated any miles in the downsampler (`cumulative_curve_dist`). No miles,
no flush, which prevents us from keeping adding data unnecessarily on every 
flush. (commit `11add0488fca`)


## How much and what data to store

### Sampling rate

For the bare display (and also for any print (vector) version we may develop)
the downsampled position (especially with a flush linked to display tick) is
sufficient - looks smooth and has the necessary detail.

For the debugging display I envision, where you can, in a script, mark out
ships/objects for monitoring we'll need the whole state of the object. One
solution would be to to grab this state on a frame-by-frame basis, storing
a state when the display asks for it.

I envision letting users scroll along the time axis to see the state of the
simulation. Positions can be interpolated but what about other aspects of state?
Do we store state only for the annotations we want, or do we
store all state in this donwsampled manner and then interpolate as needed? It's
more convenient - especially for long simulations - to just store all the state
so that we can add annotations and display them without having to re-simulate.


### Re-framing data

I allow the user to re-frame the simulation to a particular body. Different 
bodies have samples taken at different times. Linear interpolation is probably
sufficient for this - we have to figure out if there is anything to add to the
fractal downsampler for this use case. Re-framing will necessarily be inaccurate -
we have to see how this looks like visually. We have to see what this means for
annotations.


### The decision

For these considerations, and in the spirit of don't knock it till you try it:

The full state of all objects are stored whenever any aspect of state changes 
fast/non-linearly (fractal downsampler) or when a sufficient period of time 
has passed since the last sample.

If it looks like storing the whole state adds a lot of speed and memory burden
we can think about alterative strategies.


### YAGNI

The YAGNI principle is hard at work in this particular aspect of the code 
because its such a core part. I'm working hard to resist adding more features
than I currently need. My main worry is how much rewriting I'll have to do
once I need a new feature. This is teaching me to get better at modularizing
my code and resist the urge to prematurely optimize. However, I do spend time
thinking ahead to the kind of things I might like to add, and use that to plan
modularization.


## Simulation restarts and checkpoints

TBD

## Simulation

- Choosing a time step using [energy considerations][step]
- [Leap frog integration][leap]


[step]: https://physics.stackexchange.com/questions/258456/which-timestep-should-i-use-for-a-n-body-simulation-of-the-solar-system
[leap]: https://en.wikipedia.org/wiki/Leapfrog_integration
[downsampler]: ../docs/dev/adaptive-display-points.ipynb

# Interface design considerations

## Scaling data for display

If we wish for 1m resolution at solar system distances (40 AU for pluto) we need
to use doubles assuming 1 unit represents 1m. 40 AU = 5.98391483e12 m and doubles
give us integer resolution to [9007.199254740992e12 m][double-prec]. While a 
float would give us integer resolution to [16.777216e6 m][single-prec] only.

[double-prec]: https://en.wikipedia.org/wiki/Double-precision_floating-point_format#IEEE_754_double-precision_binary_floating-point_format:_binary64

[single-prec]: https://en.wikipedia.org/wiki/Single-precision_floating-point_format#Precision_limits_on_integer_values

This is fine for the simulation, but there are other considerations for the
display. OpenGL commonly accepts data as floats and the large numbers we handle
here would cause problems in terms of the matrix computations. For a fun 
discussion related to all this see [here][large-environment]. The solutions 
proposed there are complex and are needed when plotting textures and handling
movements and interactions at different scales.

For our purposes I decided that it would be easiest to scale the data before
sending it to OpenGL. It works well so far and I'll revisit it if needed.

[large-environment]: https://www.gamedev.net/forums/topic/557264-confused-very-large-environments/


## Thinking aloud on user interfaces


**The first question is, who is supposed to use this program?**

Realistically speaking, just me. So I can get away with making it as idiosyncratic
as I like


**The second question is, how much effort can we put behind this?**

This is the unhappy question. I can sketch out on napkins a very nice UI but
then who's going to build it? I do this on the train and perhaps on a few 
evenings. It's way more fun for me to program little bits of 
physics and vector math than to tussle with UI flow and components. I enjoy the 
end product of a nice UI but not the day to day struggle that goes with it. 
I enjoy and am excited by both the end product of a physics algorithm and 
the process of translating formulae and concepts into code.

### The decision

Given the limited time I have, and no other users to worry about, I chose the 
following idiosyncratic interface:

- The simulation is driven by the **simulation script** (scenario + flight plans)
- The plot of the object tracks is drawn on the **orbital diagram**
- Additional markers/information (text and graphical) can be added to the orbital diagram via one or more **annotation scripts**

I'm chosing to devote most of the effort to fun flight plan actions 
(sophisticated space craft navigation programs, interactions etc.) and, as I
work through use cases, I'll build in as annotations whatever instrumentation 
I need (e.g. a widget to monitor the relative distance between two objects 
and mark the point and value of closest approach).

Part of the assumption is that by having this kind of scripted display system I can
build things as I need them and it'll be less complex and buggy than a "reactive" 
GUI that requires callbacks and other things and perhaps a certain amount of 
upfront design and study.

But a huge part of why this script system for annotations appeals to me is that
it is drawing on my experience making graphs and plots for papers
and reports. What you really want is a script that you can version that you
run on your data to (re)produce a plot. 

What I want for Groho is a script system (the **simulation** and 
**annotation** scripts) that you can run Groho on to reproduce the exact
visuals over and over again. A reproducible semi-interactive display of the
simulation with durable annotations. 

When you have an interactive GUI a lot of it becomes ephemeral. You click on
things, you organize a display, then it's lost. Or, the program has to supply
a session system whereby you can save your widgets in a session. More complexity.

## How would annotations work?

1. An annotation is meant to show a collection of state values for an object 
(or pair of objects) and would show up as a marker with text

2. An annotation should by default track the current time point, but can be
explicitly set to appear only at one time point (and this time point can be
based on some criterion - when it is also called an event)

- Single annotation: State variable for a single object. Inverted triangle with
associated text rendered as a billboard.
- Differential annotation: difference in state variables for a pair of objects.
Rendered as line between two objects with text as billboard.

### Annotation script

_(When implemented, this section should be transfered to the tutorial section of
the main Readme)_

```

; When one id is given we show single object state
show: id:-1000 at:2458348.5 p:speed p:acc p:fuel

; When two ids are given we show differential state
show: id:-1000 id:-1001 at:2458348.5 p:speed p:acc p:fuel



show-max: id:-1000 p:speed p:acc




show: 

-- ; Annotation ends with  --
```

```
```


```
<annotation> <argument1> <argument2> ...
```

The available annotation actions are

```
Name:          show-single
Arguments:     id: integer id of body 
               at: (optional) time to display this at (don't track time cursor)
               show: (any of "speed", "acc", "fuel") (can be repeated)
               show-max: (any of "speed", "acc") (can be repeated)
               show-min: (any of "speed", "acc", "fuel") (can be repeated)
```

1. max/min/zero are local maxima and minima. For fuel, it shows when fuel went to zero

```
Name:          show-pair
Arguments:     id1: integer id of body1
               id2: integer id of body2 
               at: (optional) time to display this at (rather than track time cursor)
               show: (any of "dist", "speed", "acc") (can be repeated)
               show-max: (any of "dist", "speed", "acc") (can be repeated)
               show-min: (any of "dist", "speed", "acc") (can be repeated)
```