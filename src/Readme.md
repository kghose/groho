Developer notes
===============

<!-- TOC -->

- [Code organization](#code-organization)
    - [Flightplans](#flightplans)
- [C++ Language features](#c-language-features)
    - [Warnings, Warnings](#warnings-warnings)
        - [Everything](#everything)
    - [std::unique_ptr with forward declared type](#stdunique_ptr-with-forward-declared-type)
    - [Abstract class with non-virtual destructor](#abstract-class-with-non-virtual-destructor)
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
    - [Who does what?](#who-does-what)
- [Displaying the simulation](#displaying-the-simulation)
    - [Fractal downsampler](#fractal-downsampler)
        - [Long straight trajectories](#long-straight-trajectories)
    - [How much and what data to store](#how-much-and-what-data-to-store)
        - [Sampling rate](#sampling-rate)
        - [Re-framing data](#re-framing-data)
        - [The decision](#the-decision)
        - [YAGNI](#yagni)
    - [Multi-level detail](#multi-level-detail)
        - [The problem](#the-problem)
        - [Why not just use very dense sampling?](#why-not-just-use-very-dense-sampling)
        - [Adaptive interpolation solution](#adaptive-interpolation-solution)
        - [Multi-buffer solution (AKA The Bad Idea)](#multi-buffer-solution-aka-the-bad-idea)
        - [Integrating interpolations](#integrating-interpolations)
            - [Spliced buffers](#spliced-buffers)
    - [Simulation restarts and checkpoints](#simulation-restarts-and-checkpoints)
    - [Simulation](#simulation)
- [Interface design considerations](#interface-design-considerations)
    - [My code is getting tied to the graphics library I used](#my-code-is-getting-tied-to-the-graphics-library-i-used)
    - [Scaling data for display](#scaling-data-for-display)
    - [Thinking aloud on user interfaces](#thinking-aloud-on-user-interfaces)
        - [The decision](#the-decision-1)
    - [How would annotations work?](#how-would-annotations-work)

<!-- /TOC -->

# Code organization

* Orrery - Orrery models (NASA/JPL SPK files) loading and computation
* Configuraton - collection of scenario information and files 
* Scenario - Instantiation (loading) of scenario information in a Configuration
    - Orrery
    - Bodies (ships)
    - Actions  (State) -> (State)
* State
    - Bodies (orrery)
    - Bodies (ships)

* Simulation
    - Scenario -> State


* scenario - Scenario and flightplan loading and parsing
* simulation
    - trajectorydata
    - annotationdata
* display
    - trajectoryview
    - annotationview
* external - external libraries, currently just loguru

* buffer - code to handle data storage and sharing
* magnumapp - Visualization window based on Magnum/Corrade

## Flightplans

I wanted to be able to add new flight plans actions without having to recompile
all the code. The structure is as follows

- flightplanaction.hpp
  - contains the base class definiton
- flightplanaction.cpp
  - script parser
  - this is recompiled for each new action, we just add a forward delcaration
    for each new action



# C++ Language features

I learned several things about C++ coding while doing this project. My haphazard
notes are here.

## Warnings, Warnings

```
warning: unused parameter 'X' [-Wunused-parameter]
```
Turns out you can put `[[maybe_unused]]` in front of the variable and this
reassures the compiler that you know what you are doing ...

```
warning: catching polymorphic type 'class std::exception' by value [-Wcatch-value=]
```
From https://stackoverflow.com/a/6756040/2512851
"The reason for using & with exceptions is not so much polymorphism as avoiding slicing. If you were to not use &, C++ would attempt to copy the thrown exception into a newly created  std::exception, potentially losing information in the process."

### Everything
In a moment of distraction I put in the `-Weverything` flag into Clang. WOW.
So here are some of the fun ones

```
warning: use of old-style cast [-Wold-style-cast]
```
The old style `(float)` is more convenient than the new style `static_cast<float>`
but I guess we have to be correct ...

```
warning: constructor parameter 'ships' shadows the field 'ships' of 'State' [-Wshadow-field-in-constructor]
```
I just thought ships of State was a funny construct. Things still work fine, I
guess they want to make sure you really meant to do that.

```
warning: using namespace directive in global context in header [-Wheader-hygiene]
```
This is a good warning because you may unknowingly pass on this directive.

```
X has no out-of-line virtual method definitions; its vtable will be emitted in every translation unit [-Wweak-vtables]
```
I guess this increases the size of the executable.

```
warning: padding size of 'X' with 4 bytes to alignment boundary [-Wpadded]
```
This is strictly informational. It's interesting to see this and if you are using this
as a struct/class that ingests data from disk it concievably comes in useful
in case there are some issues related to how data is laid out on disk vs in memory.
In general, it's just clutter

```
Blah, blah [-Wc++98-compat]
```
This is a menagerie of things that c++98 doesn't like. I don't care about that
in this project because I'm using c++17. Interesting amongst these are
    - `warning: consecutive right angle brackets are incompatible with C++98 (use '> >')`
    - `warning: C++98 requires newline at end of file [-Wc++98-compat-pedantic]`


## std::unique_ptr with forward declared type

I was eager to avoid recompiling as much as possible when a new flight plan 
was added or an existing one modified. I used the classic "container of
base type pointers" pattern. It was in this context that I ran into the peculiar
issue where smart pointers are actually not *exactly* the same as regular
pointers in what will and will not compile. Specifically I ran into the issue
that I couldn't use `std::unique_ptr` with a forward declared type, but I could
use a `shared_ptr`. Howard Hinnanat has a good explanation [here][uptrfwddecl]. 

[uptrfwddecl]: http://howardhinnant.github.io/incomplete.html


## Abstract class with non-virtual destructor

On commit 1b0c8a41e8f1ffc480697583213fd4eef034640e I kept getting an `Illegal instruction: 4`
error whenever my list of flight plan actions was cleared. I first thought it
was something to do with me using `unique_ptr` and the fact that changing this
to `shared_ptr` "fixed" the problem distracted me. I thought I was incorrectly
using `unique_ptr` and there was some hidden move that I was missing.

I failed to replicate the problem using a reduced case and got pretty puzzled.
I changed things back to `unique_ptr` and a warning message caught my eye:

`delete called on 'sim::FlightPlanAction' that is abstract but has non-virtual destructor`

This led me on an internet search that revealed that, since I was using pointers
to the base class, `delete` was trying to call the (implicit) default destructor
of the base class, rather than the derived class. There is a good write up
[here][abc-dtor]. Anyhow, defining a virtual base destructor got rid of the 
warning and the bad behavior.

[abc-dtor]: https://stackoverflow.com/a/461224/2512851


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


## Who does what?

In the initial design (say upto 0.4.0 - 6ef02d21f6368) - which grew a little 
organically - the spacecraft logic (`FlightPlanAction`) was attached to the `Ship`
which was part of the `Scenario`. In order to make things work I invented a 
`WorldState` on the fly which held the orrery because I needed that for some
of the things a `FlightPlanAction` could do, as well as `Ship` initialization
like placing it in orbit.

Basically, without making a conscious decision, I had started to create spaghetti
code, where part of the state was held by `Scenario` and part by `WorldState`
and the first indication of conceptual trouble was my note for `WorldState`, which
was placed, *ad hoc* in `FlightPlanAction`, saying "Need to find a proper home for this".

In the refactor I started after 0.4.0 I explicitly made `Scenario` be the state
of the simulation and it was at that point that I realized that I had created
`WorldState` out of a sub-consious desire to avoid a circular dependency: I now
had:

`Scenario -> Ship -> FlightPlanAction -> Scenario`

since a `FlightPlanAction` altered the state of a ship while depending on world
state.

It then became apparent that, if we considered the `Simulator` to be the thing
that operated on the state (`Scenario`) then both the initialization of
the state (`Ship.init`) at the start and the application of `FlightPlanAction`
during the simulation were `Simulator` actions. This then led to the idea that
everything parsed in the scenario files related to the Ships, from static 
initial conditions, like the fuel capacity of a ship to spacecraft actions 
should be represented in the abstract as functions to be executed
by the `Simulator` - which in fact they already were - anonymous functions
except that currently they were not executed by the `Simulator` but 
during `Ship` instantiation instead.

This became very satisfying because the `orbiting` property of a ship - which
was an initial condition but which required world state had to be awkwardly
executed at the start of simulation run as an exception. This would now 
smoothly fit in with the over all philosophy.

# Displaying the simulation

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

The way to do this will probably be to create a new buffer - a re-framed buffer
- that we will use instead of the original (SSB centered one).

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


## Multi-level detail

### The problem
So, just in the earlier section I was talking about how the fractal downsampler
was good enough and a more complicated scheme YAGNI. Well looks like YGNI.

Just as I feared, as soon as I implemented a chase camera that I could re-center
on any object the big problem with the fractal downsampler cropped up - what
is smooth at one level of detail is not smooth at a sufficiently finer level
of detail. 

I first took a look at the Earth-Moon system and things looked fine there, but
when I looked at the Jovian system the issue became apparent: the moons were
swirling smoothly about a very jaggy looking Jupiter orbit. The error tolerance
of 1 million kilometers which looks fine with a whole solar system view becomes
terrible at the scale of lunar orbits.

The main issue is distance to the camera. We want more detail for objects close
to the camera and less further away. This is a typical class of problem in 
rendering large scenes.

This chase camera on Jupiter and its moons use case is extra challenging because 
the neighborhood - the region of higher detail - is continually changing. 

On the train in, I thought up a bad solution. I knew it was a bad solution because
it sounded like a whole lot of engineering - lots of rods and wheels and racks and
pinions and parts that had to fit just right. It's listed below as "The Bad Idea".
Fortunately, over lunch, I mentioned my predicament to my colleague Ivan Johnson 
and gave him the run down on my idea. He stared at me for a few seconds, 
blinked a bit and said "You might want to try cubic interpolation first, though."
He lent me Pearson's "Numerical Methods" and I read the "Interpolation 
and approximation" chapter with attention.

Now, to be honest, splines or other forms of interpolation had crossed my mind,
Perhaps it's even there in these notes in some earlier version. But I forgot
and now I was ready to go to the most complex option, without passing through
this intermediate complexity solution first. Sadly a lot of professional 
software development seems to follow this pattern too.

### Why not just use very dense sampling?
Say we want to have 1km resolution. One orbit of the Earth is 2 * pi * 1 AU
= 939,951,143 km or points. If we just store position with floats, thats 
3 x 940 million 4 byte values = 11,279,413,716 bytes ~ 11 GB for just the Earth.
If we are happy with 1000 km resolution we get to 11 MB/year which is getting to
the realm of possibility on my laptop. A complete solar system (no spaceships)
has about 180 objects so we are at ~1.9GB/year. I'm hoping I can do simulations 
for journeys or epics at least decades long,
say 20 years, which with a 1000km resolution comes to ~40GB.

There is a [quick experiment](docs/dev/sampling-density.ipynb) on the 
Mars/Phobos system here showing that 5000km sampling intervals still look good.
However, in order to reduce the memory footprint of a simulation it's the 
fractal downsampler that is the only option that gives a resonable footprint.

### Adaptive interpolation solution

1. Use the fractal downsampler to get a base level of adaptive sampling based 
   on the intrinsic curvature of the points
2. Define a neighborhood around the camera where we would like denser sampling
3. Detect segments that intersect this neighborhood and use interpolation to
   upsample the position data between these segments
4. Use time as the parameter so we can further derive velocity and acceleration
   on demand.

### Multi-buffer solution (AKA The Bad Idea)

The solution that would work here is as follows:
1. The basic simulation time step is fixed and is a small value as currently
   implemented. This is needed for basic physical fidelity. This is a simulation
   consideration and is independent of any display considerations.
2. We have a **coarse buffer** that downsamples and stores the object state at
   relatively large fixed intervals
3. We have **interpolation buffers** that are continually being computed by a second
   instance of the simulation. An interpolation buffer is triggered when one
   or more trajectory segments in the **coarse buffer** intersect the neighboor
   hood around the camera. 
4. The interpolation simulation is handed the start and stop times and the state
   of the world at the start and simply runs that part of the simulation
5. At any given time the display has a **coarse buffer** and zero or more
   **interpolation buffers**. It should seamlessly integrate the interpolation
   buffer data with the coarse buffer.

What differs here is the mechanism by which we get the data for the up-sampled
segments - rerunning a simulation is probably more complex than spline interpolation. 

### Integrating interpolations
The tricky part is how to integrate these two levels of detail.

#### Spliced buffers
One solution would be to maintain a splice list for each trajectory/path that 
indicates the indexes where we want to splice in an interpolation over an
existing path. When we go to draw a path (or retrieve data about it) we do so
upto a splice start. Then, we jump to the buffer pointed to by the splice and
use data from that instead. Once the splice is over, we come back to the
original path.

## Simulation restarts and checkpoints

TBD

## Simulation

- Choosing a time step using [energy considerations][step]
- [Leap frog integration][leap]


[step]: https://physics.stackexchange.com/questions/258456/which-timestep-should-i-use-for-a-n-body-simulation-of-the-solar-system
[leap]: https://en.wikipedia.org/wiki/Leapfrog_integration
[downsampler]: ../docs/dev/adaptive-display-points.ipynb

# Interface design considerations

## My code is getting tied to the graphics library I used

I started with FLTK. This would have done for me, except that I couldn't do
text with it (I'd have to use older versions of OpenGL) and it was pretty 
closely tied to OpenGL. Magnum is fun to use and it looks like it's a nice
abstraction on top of the actual graphics API. 

I considered the question: "What if I had to switch out Magnum for something 
else?" I decided it would be awkwardly redundant to have a layer on top of 
Mangum. In the spirit of YAGNI and trusting Magnum to it's role - an
abstraction on top of the actual graphics backend - I'm using what I need of
Magnum directly in the display component.


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

- Single annotation: State variable for a single object. Rendered as billboard 
with text just above object with small circle (also billboard) marking object point.

- Differential annotation: difference in state variables for a pair of objects.
Rendered as billboard just above a line ending in two small circles (also billboard)
marking object points. 