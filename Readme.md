Groho ( গ্রহ )
=====
Groho is a simulator for space flight, communication and warfare within the solar system.

Space is unimaginably vast. My goal is to create a simulator that will give us an 
appreciation of just how lonely even our tiny corner of the universe is. We 
define a solar system model (the Orrery - based on NASA's planetary data), 
set up space craft scripts (Flight Plans), and then sit back and watch the action 
unfold. The program makes it easy to setup, run, save, modify, inspect and 
compose (combine together) simulations. 

My hope is that we will come away with an understanding of basic orbital manuevers, 
an intuition of the time delays in communicating and coordinating across the solar 
system and a feel for what life will be like for our descendants who set sail away 
from the home planet.

*Groho (গ্রহ) is the Bengali word for planet. Grohomondol (গ্রহমণ্ডল) is the word for 
planetary system but is more of a mouthful.*

```
cd examples/001.basics
../../release_build/groho scn.groho.txt 
```
![Groho 18.07 screenshot](docs/milestone-images/groho-18.07.png "Groho 18.07 screenshot")


```
cd examples/002.full-solar-system
../../release_build/groho scn.groho.txt 
```
![Groho 18.07 screenshot](docs/milestone-images/groho-18.07-ss.png "Groho 18.07 screenshot")

[![CircleCI](https://circleci.com/gh/kghose/groho/tree/master.svg?style=shield)](https://circleci.com/gh/kghose/groho/tree/master)

<!-- TOC -->

- [Features and use cases](#features-and-use-cases)
    - [This is not an interactive simulation](#this-is-not-an-interactive-simulation)
    - [This is not an n-body simulation](#this-is-not-an-n-body-simulation)
    - [Relativistic effects](#relativistic-effects)
- [Compilation/building](#compilationbuilding)
    - [Dependencies](#dependencies)
        - [[Magnum/Corrade][magnum]](#magnumcorrademagnum)
    - [Compile](#compile)
- [Manual/Tutorial](#manualtutorial)
    - [Getting the data](#getting-the-data)
    - [Simulation files](#simulation-files)
        - [Signals](#signals)
    - [Orbit view interactions](#orbit-view-interactions)
        - [Meta](#meta)
            - [Why is the tutorial/manual in the form of commented examples?](#why-is-the-tutorialmanual-in-the-form-of-commented-examples)
            - [Why do you have stuff in the manual/examples that isn't implemented yet?](#why-do-you-have-stuff-in-the-manualexamples-that-isnt-implemented-yet)
- [Physics/astronautics word salad](#physicsastronautics-word-salad)
- [Related software](#related-software)
    - [NASA's GMAT](#nasas-gmat)
    - [NASA Ames Research Center Trajectory Browser](#nasa-ames-research-center-trajectory-browser)
    - [Rebound by Hanno Rein](#rebound-by-hanno-rein)
    - [Orbiter by Martin Schweiger](#orbiter-by-martin-schweiger)
    - [Bussard by Phil Hagelberg](#bussard-by-phil-hagelberg)
    - [SolarSystemOrbiter](#solarsystemorbiter)
    - [Poliastro by Juan Luis Cano Rodríguez](#poliastro-by-juan-luis-cano-rodríguez)
    - [Celestia](#celestia)
- [Thanks](#thanks)
    - [Components](#components)
    - [Dev tooling](#dev-tooling)
- [Meta: Why did you put everything in this one document?](#meta-why-did-you-put-everything-in-this-one-document)
    - [Some other documents of interest](#some-other-documents-of-interest)

<!-- /TOC -->

# Features and use cases

This simulator is designed to help gain intuitions of how near-future space 
flight within the solar system would look like. To this end it allows us to:

- Simulate the flights and interactions of hundreds of spacecraft
- Simulate decades long journeys in reasonable CPU time.
- Compose multiple journeys together to make a complex simulation from simpler ones
- Perform reproducible simulations (seeded random number generators, stable numerical calculations)
- Compare multiple versions of a simulation


## This is not an interactive simulation

The simulation works by setting up a scenario and then letting everything evolve 
according to physical law and scripted events. The experimenter influences the 
simulation only through the choice of scenario and script parameters.


## This is not an n-body simulation

The motions of planets, moons and larger asteroids are taken from existing 
ephemeris and are not propagated via n-body gravitational simulations. The 
expected time scales of the simulations (upto a century) are short enough that 
existing ephemeres will do fine and will save us a lot of computation. 
The gravitational effect of each planet, moon and asteroid on a ship is taken 
into account.


## Relativistic effects

Communications over solar-system distances are interestingly affected by
the finite speed of light. The simulation enables the calculation of when an event 
at one location is detected at other locations/ships.


# Compilation/building

[![CircleCI](https://circleci.com/gh/kghose/groho/tree/master.svg?style=shield)](https://circleci.com/gh/kghose/groho/tree/master)


**This code requires a C++17 compiler.** In case you need it, there is a 
[Dockerfile](.circleci/dockerfile) that installs a recent GCC.
`std::optional` is one of the C++17 features used.

## Dependencies

### [Magnum/Corrade][magnum]

You should follow the instructions on the [Magnum project page][magnum-install], 
but in brief:

For macOS:

* For the base: `brew install mosra/magnum/magnum` and `brew install mosra/magnum/corrade`
* For the plugins (required for text rendering): `brew install mosra/magnum/magnum-plugins`

[magnum]: http://magnum.graphics/
[magnum-install]: http://doc.magnum.graphics/magnum/building.html

## Compile

```
git clone git@github.com:kghose/groho.git
cd groho && \
mkdir build && \
cd build && \
cmake -DCMAKE_BUILD_TYPE=Release .. && \
make
```

# Manual/Tutorial

## Getting the data

Groho simulates the solar system using data produced by NASA/JPL. This data
is distributed by NASA as planetary kernels found [here][nasa-kernels]. 
There is a [script](examples/bsp-script.sh) under the examples directory that 
I used to pull in all the `.bsp` files for the solar system. Be aware that 
this is a few GB worth of data. You are free to use whatever kernels you wish.

[nasa-kernels]: https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/

I save all these kernels under the [`examples`][ex-dir] directory.

## Simulation files

Groho uses a **scenario file**, one or more **flight plans** and one or more 
**annotation files** to run a simulation and organize information on screen. 
Simulation files can be edited with your favorite editor and versioned with
your favourite versioning system. `groho` is independent of all this. What
`groho` does is monitor changes to the simulation files on the file system and 
updates the simulation computations when the files change.

In the [`examples`][ex-dir] directory are a series of tutorials-by-example. 
Each directory contains a scenario file, flight plans and annotation files
with a mixture of commentary and code to show you the syntax. 

[ex-dir]: examples/

Start with the [basic example][basic-ex]: Read the [simulation file][basic-sim-file],
[flight plan file][basic-flt-plan1] (there are [two][basic-flt-plan2]) and the
[annotation file][basic-ann-file].

[basic-ex]: examples/001.basics
[basic-sim-file]: examples/001.basics/scn.groho.txt
[basic-flt-plan1]: examples/001.basics/flt1.groho.txt
[basic-flt-plan2]: examples/001.basics/flt2.groho.txt
[basic-ann-file]: examples/001.basics/annotate.groho.txt

After that, just browse the examples to get a feel for what else is available.

### Signals

A flight plan can create signals on behalf of a ship. Signals travel radially 
out at the speed of light from their point of origin and can be recieved by
other ships once they are within the sphere. Once the signal has spread past 
the furthest simulation object it is removed, since all ships have recieved
the signal by that time.


## Orbit view interactions

Dragging the mouse will orbit the camera. Scrolling will zoom in and out. 
Pressing ALT while scrolling will move back and forth in time. The left/right 
cursor keys cycle through the planets/asteroids/spacecraft and the up/down keys 
cycle through the satellites of said planets allowing us to center the camera on 
different objects. 

Once you have sufficient objects in the simulation, it becomes hard to find things. 
Whatever view you can get to using the mouse/keyboard, you can also get to using
the `view` annotation and in a much more effective way. Check out the documentation.

*Eventually the [tutorial here](docs/tutorial.md) will be copied over into this space.*

### Meta 

#### Why is the tutorial/manual in the form of commented examples?

I personally learn well by example, so I wanted to try this out. I also use
these tutorial scripts as ways to prototype proposed simulation file syntax,
to see how it could look and work.

Lastly, for a hobby project, it's a wasteful duplication of effort to have to 
create/update example scripts and a separate manual or tutorial 
(where I'd have to paste in snippets of code anyway). I chose to use comments 
in the simulation files as a reliable way to keep upto-date documentation. 

#### Why do you have stuff in the manual/examples that isn't implemented yet?

I'm aspirational. But seriously, I use the manual and the example input files as
functional specifications. As I build out more of the software, more of the
specifications are implemented. Please file bug reports as necessary.


# Physics/astronautics word salad

There is a bunch of interesting physics and math behind orbital maneuvers.
From Robert A. Braeunig's [page][bob] we have the following named maneuvers:

- Hohmann transfer orbit
- One-Tangent Burn
- Spiral transfer
- Simple plane change

[bob]: http://www.braeunig.us/space/orbmech.htm#maneuver

And, from [poliastro]

- Lambert’s problem
- Bi-elliptic transfer


# Related software


## NASA's GMAT

NASA's [GMAT] software ([github][GMAT-github]) is probably the be-all and end-all 
of spacecraft simulation. It would probably have sufficed for my needs 
but I like to program computers and learn math, 
so I stubbornly rolled my own to my own (much more limited) specifications.

[GMAT]: http://gmatcentral.org/display/GW/GMAT+Wiki+Home
[GMAT-github]: https://github.com/haisamido/GMAT

## NASA Ames Research Center Trajectory Browser

I ran into the [Trajectory Browser] while looking for what interplanetary
mission programs look like. It's awesome. Give it a spin.

[Trajectory Browser]: https://trajbrowser.arc.nasa.gov/traj_browser.php

## Rebound by Hanno Rein

[REBOUND] is an N-body integrator. Groho completely punts on this aspect of the
simulation and relies on the NASA/JPL ephemeris kernels (which in turn are
products of a constrained N-body simulation). The REBOUND code is a good place
to explore different integrators and the rebound paper is a good place to
readup about Symplectic integration. If you are into that kind of thing.

[REBOUND]: https://github.com/hannorein/rebound

## Orbiter by Martin Schweiger

I've used [Orbiter](http://orbit.medphys.ucl.ac.uk/) since 2001 or so. It was
my main instrument of thesis procrastination. It's very detailed and visually
rich and a blast to play. It's an interactive simulation where you fly space
craft with your keyboard and mouse. It's closed source and its internals are 
a little opaque. For example, I do not know
what orrery model it uses or if it does an n-body simulation, if it has
accurate models for the axial tilt of the planets and moons or these are
just arbitrary and so on.

## Bussard by Phil Hagelberg

[Bussard](https://technomancy.itch.io/bussard) is a cool hacking/spaceflight game
where you write complex programs to fly your spaceship to solve puzzles.

## SolarSystemOrbiter

[SolarSystemOrbiter](https://github.com/madoee/SolarSystemOrbiter) -
"Plot the orbits of the planets in our Solar System and calculate the Hohmann Transfer Orbits to transfer your rocket ship from one planet to the other and back." 

## Poliastro by Juan Luis Cano Rodríguez

[poliastro] is an open source collection of Python subroutines for solving 
problems in Astrodynamics and Orbital Mechanics.

[poliastro]: http://docs.poliastro.space/en/latest/index.html

## Celestia

[Celestia](https://github.com/CelestiaProject/Celestia) - 
"A real-time space simulation that lets you experience our universe in three dimensions."

# Thanks

## Components
Included in the code are the following fine pieces of software

1. [loguru](https://github.com/emilk/loguru) from Emil Ernerfeldt for the logging
2. [catch(2)](https://github.com/catchorg/Catch2) from Phil Nash for unit tests
3. [magnum/corrade][magnum] from Mosra (Vladimír Von­druš) for everything graphical

## Dev tooling
I was helped in writing the code by

1. [The Clang compiler][clang] and `clang-format`  
1. [Visual Studio Code][vs-code] from microsoft which does everything 
   I want in a code editor (Though the debugger integration needs some work.)
1. [Instruments]. I dislike Xcode, but love Instruments
1. [markdown-toc][mtoc] from Alan Walk. I used that to generate the 
   table of contents in markdown documents, which allowed me to 
   consolidate my documentation.


[vs-code]: https://code.visualstudio.com/
[clang]: https://clang.llvm.org/
[Instruments]: https://help.apple.com/instruments/mac/current/
[mtoc]: https://github.com/AlanWalk/Markdown-TOC


# Meta: Why did you put everything in this one document?

When I started the project I had text documents all over the place. It seemed
like a good idea - one document for an idea or topic. Pretty soon I had multiple
docments with similar ideas/topics and I began to forget that I had them.
Discoverability began to be a problem. So I decided to try packing stuff into
as few documents as possible. You don't have to read all this at once, you can
look for things using your browser or text editor's search function and not
have to keep jumping from document to document. I will try to use bookmarks
to help orientation. It's an experiment - tell me if you hate it.

## Some other documents of interest

That said, there are a few other documents that you may be interested in from
a development point of view. I kept them separate because they are not necessary
to operate Groho.

- [Notes about C++ and computations](src/Readme.md) - Just some war stories and C++ notes
- [Examples](examples) - These are example scenario files to get you started
- [backlog](docs/backlog.md) - some plans and ideas