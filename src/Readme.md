Developer notes
===============

<!-- TOC -->

- [Notes on C++](#notes-on-c)
    - [std::pair and std::lock_guard](#stdpair-and-stdlock_guard)
    - [structured bindings](#structured-bindings)
    - [std::map [] and const : gcc vs clang](#stdmap--and-const--gcc-vs-clang)
    - [Templated class specialization where template argument is a template](#templated-class-specialization-where-template-argument-is-a-template)
    - [Explicit template instantiation](#explicit-template-instantiation)
    - [Template template arguments](#template-template-arguments)
    - [Order matters](#order-matters)
    - [Instrumentation/profiling/debugging](#instrumentationprofilingdebugging)
        - [`-fsanitize=address`](#-fsanitizeaddress)
        - [Profiling on mac](#profiling-on-mac)
    - [Warnings, Warnings](#warnings-warnings)
        - [hides overloaded virtual function [-Woverloaded-virtual]](#hides-overloaded-virtual-function--woverloaded-virtual)
        - [warning: all paths through this function will call itself [-Winfinite-recursion]](#warning-all-paths-through-this-function-will-call-itself--winfinite-recursion)
        - [warning: unused parameter 'X' [-Wunused-parameter]](#warning-unused-parameter-x--wunused-parameter)
        - [warning: catching polymorphic type 'class std::exception' by value [-Wcatch-value=]](#warning-catching-polymorphic-type-class-stdexception-by-value--wcatch-value)
        - [Everything](#everything)
    - [std::unique_ptr with forward declared type](#stdunique_ptr-with-forward-declared-type)
    - [Abstract class with non-virtual destructor](#abstract-class-with-non-virtual-destructor)
    - [Code formatting](#code-formatting)
    - [Mutable](#mutable)
    - [Optional](#optional)
    - [Switch-board (Pythonic C++)](#switch-board-pythonic-c)
    - [Building](#building)
        - [Compile an individual file](#compile-an-individual-file)
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
- [Some notes on implementation](#some-notes-on-implementation)
    - [Organization](#organization)
    - [Loading planetary kernels](#loading-planetary-kernels)
    - [The LOD problem](#the-lod-problem)
        - [Option: Store all the data](#option-store-all-the-data)
        - [Option: Adaptive downsampling](#option-adaptive-downsampling)
        - [The LOD problem](#the-lod-problem-1)
            - [More power is not the answer](#more-power-is-not-the-answer)
        - [Adaptive sampling + interpolation](#adaptive-sampling--interpolation)
        - [Loss of fidelity](#loss-of-fidelity)
    - [Restarts](#restarts)
        - [Composing simulations](#composing-simulations)
        - [Restarts and flightplans](#restarts-and-flightplans)
    - [Do we need restarts?](#do-we-need-restarts)
    - [Sharing a data buffer between writer and reader](#sharing-a-data-buffer-between-writer-and-reader)
    - [Flightplans](#flightplans)
        - [Code organization](#code-organization)
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

# Notes on C++

I learned several things about C++ coding while doing this project. My haphazard
notes are here.

## std::pair and std::lock_guard

While working on 

The following will compile and execute fine:
```
#include <iostream>
#include <mutex>
#include <utility>
#include <vector>

template <typename T> class Locked {
public:
    Locked(T& _object, std::mutex& _mutex)
        : object(_object)
        , lock(_mutex)
    {
    }

    T&                          object;
    std::lock_guard<std::mutex> lock;
};

template <typename T> class Lockable {
public:
    Locked<T>       borrow() { return Locked(object, mutex); }
    Locked<const T> borrow() const { return Locked(object, mutex); }

private:
    T                  object;
    mutable std::mutex mutex;
};

int main()
{
    Lockable<std::vector<int>> lv;
    
    auto [vec, lock] = lv.borrow();

    std::cout << vec.size() << std::endl;
    return 0;
}
```


While this subsequent code, will throw up a horrendous, hard to parse error.

```
#include <iostream>
#include <mutex>
#include <utility>
#include <vector>

template <typename T> class Lockable {
public:
    std::pair<T&, std::lock_guard<std::mutex>>       borrow() { return std::pair(object, std::lock_guard<std::mutex>(mutex)); }
    std::pair<const T&, std::lock_guard<std::mutex>> borrow() const { return std::pair(object, std::lock_guard<std::mutex>(mutex)); }

private:
    T                  object;
    mutable std::mutex mutex;
};

int main()
{
    Lockable<std::vector<int>> lv;
    
    auto [vec, lock] = lv.borrow();

    std::cout << vec.size() << std::endl;
    return 0;
}
```
I think it has to to with std::lock_guard and move semantics but I fail to
understand why, then, other version works. In the working version, in my mind,
the `Locked` class is bascially same as `std:pair`.


## structured bindings

C++17 introduced structured bindings which can be used to extract components of
a pair or tuple very neatly:

```
auto [a, b] = std::pair("hi", 23);
for( auto [key, value] : my_map ) { ; }
```

What I find especially cool is that such bindings "just work" with structs and
classes, extracting out, in order, all the public data members of a class.

```
struct A {
    int a;
    float b;
};

A my_A{10, 12.5};

auto [a, b] = my_A;
```

More details are [here](https://blog.tartanllama.xyz/structured-bindings/)


But there are wierdnesses with structured bindings that one runs into when using
lambdas
https://stackoverflow.com/a/46115028/2512851


## std::map [] and const : gcc vs clang

(Thanks to http://www.cplusplus.com/forum/beginner/160820/)

The `[]` operator on `std::map/unordered_map` will perform an insert if the key 
does not exist. So one can not use this in a function where the map is supposed
to be a constant. Except, clang happily compiles such code where as gcc complains.
The replacement is to use `at`. 


## Templated class specialization where template argument is a template

(From a [question](https://stackoverflow.com/questions/4189945/templated-class-specialization-where-template-argument-is-a-template) and [answer](https://stackoverflow.com/a/4200397) on stack overflow)

```
template <typename T> struct SnapShot {
    typename T::Property property;
    typename T::State    state;
};

template <typename T> struct BaseCollection {
    std::vector<T>                       bodies;
    ...
};

template <typename T> struct Collection : public BaseCollection<T> {
};

template <>
template <typename T>
struct Collection<SnapShot<T>> : public BaseCollection<SnapShot<T>> {
    ...
};
```


## Explicit template instantiation

A common refrain on the internet is that you must put template class/function 
implementations in the header file itself ([e.g.][so-tplt-hdr]).

This is of course annoying but there is a way to get around this, that is not so
bad and it is called [explicit template instantiation][cpp-tplt-hdr]. You split
your class/function template into header and implementation as you normally would.
Then, after all the functions have been implemented, in the implementation file
you create a list of explicit instantiations of the template with all the template
type arguments you will need. In my case, I had a class with templated member
functions, and at the end of my implementation file I had:

```
template void
Path::copy_all<RockLike::State>(const SubBuffer<RockLike::State>& buf_data);
template void
              Path::copy_new<RockLike::State>(const SubBuffer<RockLike::State>& buf_data);
template void Path::map<RockLike::State>(
    const SubBuffer<RockLike::State>& buf_data, Mode mode);

template void
Path::copy_all<ShipLike::State>(const SubBuffer<ShipLike::State>& buf_data);
template void
              Path::copy_new<ShipLike::State>(const SubBuffer<ShipLike::State>& buf_data);
template void Path::map<ShipLike::State>(
    const SubBuffer<ShipLike::State>& buf_data, Mode mode);
```

Dr. Dobb's [article][dobbs-tplt-hdr] has a good discussion on the merits and de-merits
of this approach.

What was very interesting was to learn of the ill-fated c++ keyword: `export`.
[This paper][export-pdf] is racy reading (for a standards group document) and
this [SO thread][so-export] is also informative.

From advice in a Dr. Dobb's [article][dobbs-tplt-hdr] and the [cppreference][cpp-tplt-hdr]

[so-tplt-hdr]: https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
[dobbs-tplt-hdr]: http://www.drdobbs.com/moving-templates-out-of-header-files/184403420
[cpp-tplt-hdr]: https://en.cppreference.com/w/cpp/language/class_template

[export-pdf]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2003/n1426.pdf
[so-export]: https://stackoverflow.com/questions/279404/what-is-the-best-explanation-for-the-export-keyword-in-the-c0x-standard


## Template template arguments

(https://en.cppreference.com/w/cpp/language/template_parameters)
```
template <typename T> struct Simulation {
    typename T::Property         property;
    SubBuffer<typename T::State> history;
};

template <typename T> struct SnapShot {
    typename T::Property property;
    typename T::State    state;
};

template <template <typename> class T> struct Objects {
    std::vector<T<RockLike>> system;
    std::vector<T<ShipLike>> fleet;
};

...

Objects<Simulation> simulation;
```

where

```
struct RockLike {
    struct Property { ... };
    struct State { ...};
};

struct ShipLike {
    struct Property { ... };
    struct State { ... };
};
```

Very impressively clang's error message is very useful
```
error: template argument for template
      type parameter must be a type; did you forget 'typename'?
```

## Order matters

In a block of code the order of destruction of objects is the reverse of their
creation which is simply their order of declaration. In a normal block of code
I'm cognizant of imperative order because, well, that's imperative code. In
a class declaration, however, I don't pay that much attention to the order of
my definitions unless I'm worrying about serialization.

I tend to think
```
class A {
    int x;
    float y;
    std::string z;
};
```
is equivalent to
```
class A {
    std::string z;
    int x;
    float y;
};
```

But that is not exactly true. When class A is destroyed the destruction of the
objects in the first case is `z, y, x` and in the second case is `y, x, z`. In
most cases it does not matter except when it mysteriously does.

My original code was

```
    ... {
        _font = _manager.loadAndInstantiate("FreeTypeFont");

        ...
    }

    ...

    PluginManager::Manager<Text::AbstractFont> _manager;
    std::unique_ptr<Text::AbstractFont> _font;
    Text::DistanceFieldGlyphCache       _cache;
```

I decided to refactor some stuff and separate out the private and public parts.
For aesthetic reasons I put the private bits last ...

```
public:
    std::unique_ptr<Text::AbstractFont> _font;
    Text::DistanceFieldGlyphCache       _cache;
private:
    PluginManager::Manager<Text::AbstractFont> _manager;
```
This lead to 
```
Loguru caught a signal: SIGABRT
Stack trace:
7                  0x2 11  ???                                 0x0000000000000002 0x0 + 2
6       0x7fff5288b015 start + 1
5          0x10b1bd093 main + 675
4          0x10b1b1c0e sim::GrohoApp::~GrohoApp() + 190
3          0x10b588945 Corrade::PluginManager::AbstractManager::~AbstractManager() + 115
2          0x10b588b7f Corrade::PluginManager::AbstractManager::unloadRecursiveInternal(Corrade::PluginManager::AbstractManager::Plugin&) + 305
1       0x7fff529371ae abort + 127
0          0x1173950a8 4   ???                                 0x00000001173950a8 0x0 + 4684599464
2018-07-19 00:20:20.528 (   2.023s) [main thread     ]                       :0     FATL| Signal: SIGABRT
Abort trap: 6
```
At exit. Of course, as is usually the case, I had made a bunch of other changes
with this and it took me a while to zero in on what was actually going on.

Restoring the imperative order
```
private:
    PluginManager::Manager<Text::AbstractFont> _manager;

public:
    std::unique_ptr<Text::AbstractFont> _font;
    Text::DistanceFieldGlyphCache       _cache;
```
restored order (and gave me the opportunity to make a subtle Darth Vader related pun)


## Instrumentation/profiling/debugging

### `-fsanitize=address`
Address sanitizer `-fsanitize=address` is set for debug compiles and LeakSanitizer
can be run by doing, for e.g. `ASAN_OPTIONS=detect_leaks=1  ../../debug_build/groho scn.groho.txt --no-gui`

### Profiling on mac

This is remarkably simple and shows why the tooling on mac is considered very
good - I used Instruments -> Time Profiler to find code chokepoints and Leaks
for memory issues.

## Warnings, Warnings


### hides overloaded virtual function [-Woverloaded-virtual]

(from https://stackoverflow.com/questions/18515183/c-overloaded-virtual-function-warning-by-clang)
This warning is there to prevent accidental hiding of overloads when overriding is intended. 

Another way of disabling the warning keeping the struct public interface intact would be:
```
struct Derived: public Base
{
    virtual void * get(char* e, int index);
private:
    using Base::get;
};
```
This disallows a consumer of Derived to call Derived::get(char* e) while silencing the warning:

```
Derived der;
der.get("", 0); //Allowed
der.get("");    //Compilation error
```

### warning: all paths through this function will call itself [-Winfinite-recursion]

Rather, this taught me about the `::` for global scope. I had code that basically
looks like this:

```
#include <iostream>

void foo() { std::cout << "::foo"; }

struct A {
    void foo()
    {
        std::cout << "A::foo";
        foo(); // <-- !!!!
    }
};

int main() { return 1; }
```

`foo()` in the member function refers to the member function (hence the 
recursion warning). What is needed is:

```
    void foo()
    {
        std::cout << "A::foo";
        ::foo();
    }
```
to indicate global scope

https://stackoverflow.com/questions/7149922/calling-a-global-function-with-a-class-method-with-the-same-declaration


### warning: unused parameter 'X' [-Wunused-parameter]

Turns out you can put `[[maybe_unused]]` in front of the variable and this
reassures the compiler that you know what you are doing ...

### warning: catching polymorphic type 'class std::exception' by value [-Wcatch-value=]

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

### Compile an individual file

(https://stackoverflow.com/a/41262107)
Find the internal make file CMake generates
```
cd build_directory
grep "my_file" *
```

Then inspect it to find the correct target and then you will end up doing something
like
```
make -f CMakeFiles/groho.dir/build.make CMakeFiles/groho.dir/src/orrery/spkorrery.cpp.o
```

This is useful for major refactors when everything is flux and it's more manageable
to compile one file at a time and go that way.

The other useful thing is not to do a major refactor ...


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

But [see this](#explicit-template-instantiation)


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


# Some notes on implementation

## Organization

Simulation contains Scenario, History and State

Scenario loads and contains orrery, flightplans and configuration (file I/O)




## Loading planetary kernels

It was a good decision to use the authoritative ephemeris from NASA/JPL for the 
orrery - there is a wealth of accurate data available is the SPK format and it
was entertaining (to an extent) and satisfying to analyze the data structure and 
then implement a reader for it. It was especially satisfying to see the complex
non-planar orbits of the moons of the gas giants and a lot of fun to load up the
SPK file carrying those 343 asteroids and see them swirl around the sun.

## The LOD problem

My goal is to be able to run the following simulation:

**Simulation: 60 bodies (full solar-system, no asteroids/comets) + 20 spaceships, 
for a time span 100 years**

On my current laptop:

**Machine specs: 16GB, 2.7 GHz Intel Core i5 (2 cores)**

The raw compute is not so much of an issue, especially given that it's not an
interactive simulation. What is challenging is the sheer size of the data
produced which is unwieldy to store and display.

### Option: Store all the data

Assuming a simulation time step of 1 minute a 100 year simulation will result in
100 * 365.25 * 24 * 60 = 52.6 million data points per trajectory. If we store 
just the position and we stpre this as floats, this is 52.6e6 * 3 * 4 = 631 MB
of data per trajectory. For our full-solar stsem simulation this is 12.6 GB for 
the 20 spaceships alone. 

### Option: Adaptive downsampling

A 1 minute interval, possibly essential for numerical stability, is probably
way more dense than necessary for display purposes. Downsampling the data allows
us to maintain visual fidelity. For reasons detailed in this 
[IPython notebook][downsampler] a decent solution is an adpative downsampler
that samples tighter curvatures more densely. 

This can be tuned to give a density of sampling such that,
at a scale where the whole, or substantial parts of, the solar system are visible, 
the trajectories are visually pleasant to look at even with just linear interpolation.
This allows us, in practice, to reduce a 100 year, full solar system simulation
to around 2-3 million points.

### The LOD problem

Solving this problem graduates us to the next level of problem: The Level of Detail
issue. When you zoom in however, say to look at Mars and it's moons, downsampled data 
with linear interpolation looks very annoying. A smooth trajectory like mars
has sampling intervals that are large compared to the tight motions its moons
make, resulting in a very glitchy and inaccurate rendering with linear interpolation.

> ### Mars, Phobos and Deimos
> The orbits of Phobos and Deimos are particularly challenging - they are both
strikingly close to Mars and their orbits correspondingly have tight curvatures.
Visually, aggressive downsampling works for most of the planets/moons but not for the Martian system.

#### More power is not the answer
I tried cranking down the tolerances on the adaptive sampler but the problem
is that to get dense enough sampling for the Martian system we start to flood
with data. With the default tolerances, for a 100 year full solar system simulation
we end up with about 2.6 million points. This can skyrocket to 31 million points
and we still have glitchy visuals.

It's possible to set the tolerances differently for the different planets, since
only the Martian system has such a tight scale, but we're just kicking the can
down the road and we're going to run into again, for example, when we'll be
looping a spacecraft through the Jovian system and wonder why everything looks
so funny. 

### Adaptive sampling + interpolation

The LOD problem hits us when we zoom in to take a closer look at a region of
space. One way to mitigate this is to adaptively sample the simulation to reduce
the data volume and then interpolate (upsample) dynamically, as needed, those
parts of the traces that we are looking closely at. The scheme is as follows:

- Use adaptive sampling to reduce data volume
- Define a volume around the camera target center called the neighborhood. When 
  the camera gets within this neightborhood, interpolate.
- For a given camera distance sample all trajectories at a certain density and
  upto a certain spatial extent within the neighborhood
- The Orrery is recomputed (I'm assuming that, since this is already an interpolation
  this will be as fast (or faster that) cubic interpolation)
- The space craft trajectories are interpolated using a cubic spline 
- Show at most one orbit of any body around the sun/SSB at any given time. 
  This is because showing multiple orbits clutters up the display unnecessarily.
  We can do this by walking backwards from the current time cursor and stopping 
  plotting when we describe an angle of 2pi
- The interpolation should run in a separate thread, triggered by camera movement
- We should optimize for the case where the user zooms in and then moves forward/backward
  in time - so we should be smart and not recompute overlapping sections
- It should be possible to cancel the interpolation computation and restart -
  for the case where the user changes the camera target

This scheme has the following limitations that we will see if we can live with
- We can position the camera such that there are objects and paths very close
  to the camera, but far from the central body. No interpolation will be triggered
  for these close bodies

### Loss of fidelity

We have to accept that for annotation purposes the spacecraft component of
positions and velocities will be approximate, given that they are interpolations.
However, for the purposes of what we use them for - designing flightplans and
gaining intuitions of spaceflight, it should be fine. Also, precise annotations,
like marking maximum distance, or maximum closing velocity will be marked from
the actual data, as the event happens, so these will be exact.  

## Restarts

In the distant future we have some notion that we would like to do restarts.
Either we want to be able to inspect and edited scenario and determine if we can
reuse previous computations, or we want to load a saved simulation and extend it.
Right now (2018) this is beyond the scope of a 18.12 release but we perhaps we
can plan ahead.

An important thing with restarts is what exactly can we restart? A restart
requires us to save the state history upto a point in time, and then continue
the simulation from that point. But important parts of the simulation need to be
identical.

### Composing simulations
- Run simulations of two ships separately and then combine them together as one
  - It would be nice to save on the computations, but how?
- Run simulations upto a point in time, then extend these simulations to later
  epochs without changing events earlier in time. How do we determine this?
  - Initial conditions have to be identical
  - Actions can only change after a ceratain point in time

It sounds like restarts are going to be complex because we anticipate changing
parts of the simulation. What we will restrict ourselves to here is designing
the flightplan in a way that will enable restarts in the future, even though
we anticipate this being very hard to do re: figuring out what we can restart
given a changed scenario.

### Restarts and flightplans

For restarts to work, we need flight plans to be either stateless or we should 
have well defined state vectors for the flight plans that can be saved. A 
convenient thing may be to collect any state variables
into a class specific struct (say `State`). Down the road we might be able to
serialize the flight plan by writing out this `State` in an orderly fashion.
Currently we remove flightplans that are done, so presumably we'll need to
preface each state save with a serial number and type of the flightplan, so
we can reconstitute it when we load it back.

## Do we need restarts?

We are talking about caching computations here, and the question is whether we
really need it?

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


## Flightplans

Flight plan actions can not change global state during regular running. 
This is a matter of philosophy: We could say "An action can do whatever we want" 
and let it do things like teleport other ships (by changing their position 
abruptly). However, in the spirit of the simulation an action should really 
only be able to change the thrust (acc + att) of a single spaceship.
    
What about if this is a swarm of spaceships, you ask? Why can't I set the 
states of multiple-spaceships directly? The proper solution is to broadcast a 
signal that these other spaceships will then catch. Remember, no faster than 
light travel or communications are allowed.

It's good to have constraints like this - it enforces the physical law we want
to obey, and it makes us creative 
    
During initial setup, a flightplan still has access to altering global state 
because that is still part of how we set up initial conditions.

### Code organization
I wanted to be able to add new flight plans actions without having to recompile
all the code. The structure is as follows

- flightplanaction.hpp
  - contains the base class definiton
- flightplanaction.cpp
  - Carries script parser
  - We add a forward declaration here for each action.
  - This has to be recompiled when a new action is added
- actionlib.cpp/.hpp
  - some common functions shared just by actions
  - a change here will cause a recompilation of any action that uses this lib
- actionX.cpp
  - carries a new action, recompiled when code is changed


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

Our goal is to run a 100 year simulation with multiple ships operating across the
whole solar system. Just as an assumption probably a 60s time step will suffice for


## Fractal downsampler

The exciting story behind the fractal downsampler is in this [IPython notebook][downsampler].

### Long straight trajectories

The downsampler works great and gives us all the benefits we hoped for, but
there are some aesthetic and scientific issues related to straight trajectories.
A spacecraft flying with high constant acceleration has a very straight 
trajectory and the display will be jerky. 

~The solution chosen is to force a flush of the buffer when the display 
asks to see it. Whether we have anything to flush depends on whether we've
accumulated any miles in the downsampler (`cumulative_curve_dist`). No miles,
no flush, which prevents us from keeping adding data unnecessarily on every 
flush. (commit `11add0488fca`)~

The problem with this solution is that it in-elegantly ties display refresh to
simulation data sampling. ~In the current implementation, because of how 
inefficiently we mirror the simulation buffer to the display buffer, this can
cause a strange feedback loop where more and more simulation points are sampled
because it takes longer and longer to mirror the data.~ I've made the data
mirroring more efficient, but in principle this problem remains


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