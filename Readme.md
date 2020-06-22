Groho ( গ্রহ )
=====
Groho is a simulator for space travel within the solar system.

[![Tests](https://github.com/kghose/groho/workflows/Tests/badge.svg)](https://github.com/kghose/groho/actions?query=workflow%3ATests)
[![Simulator](https://github.com/kghose/groho/workflows/Simulator/badge.svg)](https://github.com/kghose/groho/actions?query=workflow%3ASimulator)

Even our tiny corner of the universe, the solar system, is unimaginably vast and
lonely. Using this simulator, I hope we can gain an intuition and appreciation
for basic orbital manuevers, and a sense of scale and time for travel between the
planets. In this way we can get a feel for what life will be like for our
descendants who set sail away from the home planet 

*Groho (গ্রহ) is the Bengali word for planet. Grohomondol (গ্রহমণ্ডল) is the
word for planetary system but is more of a mouthful.*


# Quick start

**You can put the compiled code, your scenario files and the data files
anywhere on disk. For simplicity, in the tutorial here, especially so that the
example scenario file can work out of the box, I mention a fixed directory
structure.**

## Installation

_I haven't had time to make things super easy. This assumes some knowledge of
how to compile C++ programs and install Python programs with `pip`_

The software consists of two parts: a simulator, which is a C++ program that
needs to be compiled, and a visualizer, which is a Python program that should be
installed.

You need `cmake` 3.12.0 or later, a c++ compiler that can do C++17, like `gcc`
9.3.1 or later, or `clang` 11.0.3 or later, and Python 3.7 or later.

### Grab the code
Best is to `git clone` the latest code from the `stable` branch.
```
git clone https://github.com/kghose/groho.git
```

### Install the visualizer
From the root of the code directory, where you find this Readme.md file, in
a Python 3.7 environment 
```
pip install .
```

### Compile simulator
From the root of the code directory, where you find this Readme.md file: 
```
mkdir build
cd build
cmake ..
make -j4
```

## Pull sample data files

The simulator loads planetary and satellite data from data files put out by the
Jet Propulsion Laboratory (JPL). These are called SPICE kernels and many can be
found from [this website](https://naif.jpl.nasa.gov/naif/data.html)

The example uses the [de432s.bsp] kernel. Download this file to the `examples`
directory. 

[de432s.bsp]: https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/planets/de432s.bsp


## Running

Open a terminal and, from the root directory, do
```
build/groho sim examples/basic-scenario.txt simout
```
This will start the simulator in "server" mode, which just means it runs the
simulation and then sits watching for changes to the scenario described by
`basic-scenario.txt` and reruns whenever the scenario is changed.

In another terminal, in your Python 3.7 environment, start the visualizer code
with 
```
grohoviz simout exmples/basic-plotfile.txt
```
This starts the visualizer which will plot the simulation and then wait,
watching for updates to the simulator output (which it will replot
automatically) and for updates to the plotfile, which it will also replot,
automatically. 

At this point you should have the following plot:


- plot goes here -

The `groho` executable has a few other modes that the CLI help will explain to
you if you are interested. 

# Scenario file manual

The scenario file lists out the initial conditions for the simulation, the orrery
model and the flight plans for any spacecraft in the simulation.

The easiest way to learn about scenario files is to look at the annotated
[example](examples/basic-scenario.txt) we just ran.

To get a list of spacecraft programs and how to use them do
```
build/groho programs
```

## Orrery model
You pass a list of kernel files to the simulator. Optionally, you can indicate a
subset of NAIF codes that the simulator should load from the kernel file.

```
spk de432s.bsp ; load everything from this file

pick 809 899
spk nep086.bsp ; only load 809 and 899 from this file
```

A SPK file contains barycenters as well as physical bodies. Often ephemeris are
stored relative a barycenter. For example:
```
301 -> 3 -> 0
299 -> 2 -> 0
809 -> 8 -> 0
899 -> 8 -> 0
```

When loading objects the simulator follows the following rules

1. If a barycenter and the main body of the barycenter are both loaded, the
   barycenter is used for coordinate transforms but is not used for gravity
   computations: the main body is used. The trajectory of the barycenter is
   also, in this case, not saved.
1. If a body encountered more than once, the ephemeris is loaded from the first
   file it is encountered in.

In the given example, the barycenter 8 is not used for gravity computations
since 899 is loaded. 


## Flight plans

Flight plans start with a line indicating the name of the spacecraft

```
plan Durga
```

This is followed by a list of **events**. Each event specifies a start time,
duration and, a spacecraft **program** and how it should run. For example the
line: 

```
2050.01.01:0.5 3600 orbit 301 200x200
```

will turn on a program that thrusts the spaceship till it achieves a 200x200 km
orbit around the moon or till the duration is up, whichever is earlier. (A
duration is required because this makes restarts easier. It allows the simulator
to more easily decide the time point upto which computations can be reused)

Each program has access to the state of the solarsystem (modeling a perfect IMU
and perfect knowledge of the solar system) and produces only one output: a
thrust vector for the spaceship.

Multiple programs can not run at the same time. If a program's event time occurs
before the previous program has terminated, it will be put into a
queue, so that they will run as soon as the earlier program has finished.

To describe another spacecraft's flight plan, simply use another `plan`
statement. All events coming after this, will be associated with this new
spacecraft. 

## The `insert` directive
`insert` followed by a file path inserts the text of that file into the original
file at that point. This can be done recursively. In this manner, multiple files
can be combined to form the complete scenario file. For example 

```
start 2050.01.01:0.5
end 2055.01.01:0.5

spk de432s.bsp
pick 809 899
spk nep086.bsp 

insert flightplan1.txt
insert flightplan2.txt
```
is a neat way of splitting out the flightplans of the two spacecraft in the
simulation into two additional files. This can make the writing of the
simulation more manageable.

# Plot file manual
**WIP**


# Getting the data


## Physical constants
The webpage at https://ssd.jpl.nasa.gov/sbdb_query.cgi#x will generate CSV files
with customized data as you need.


# Developer documentation
I use this project to keep current with my design and C++ skills. 

- Here are some [UML design documents](docs/Readme.md)
- My relevant [blog posts][posts] on C++ and other topics.
- Some very [unorganized notes](src/Readme.md) on C++ used in this project. 

[posts]: https://kaushikghose.wordpress.com/tag/spacecraft-trajectory-simulator/


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

There is a big list at https://github.com/orbitalindex/awesome-space

## NASA's GMAT

NASA's [GMAT] software ([github][GMAT-github]) is probably the be-all and end-all 
of spacecraft simulation. It would probably have sufficed for my needs 
but I like to program computers and learn math, 
so I stubbornly rolled my own to my own (much more limited) specifications.

[GMAT]: http://gmatcentral.org/display/GW/GMAT+Wiki+Home
[GMAT-github]: https://github.com/haisamido/GMAT

## Solar System Voyager (SSVG)

"[SSVG] is simulation software which enables users to fly their own space probes in the solar system.  Each space probe has three propulsion systems: a chemical propulsion engine, an electric propulsion engine, and solar sail.  Fly your own space probe in the precisely simulated Solar System."

I found SSVG on Oct 14th and browsing through the manual][SSVG-manual] found that
the author had a very similar concept (including the name "Flight plan") and a
cool looking GUI for writing flight plans! I had, by then, however, discarded the
idea of timed actions in favor of event/condition driven actions. But it is
cool for me to see a "flight plan" like concept, because I have so far been
unsuccessful in finding what a real flight plan, or set of propulsive maneuvers
actually looks like.

[SSVG]: https://github.com/whiskie14142/SolarSystemVoyager/
[SSVG-manual]: https://github.com/whiskie14142/SolarSystemVoyager/blob/master/doc/SSVG_UsersGuide-en.pdf


## NASA Ames Research Center Trajectory Browser

I ran into the [Trajectory Browser] while looking for what interplanetary
mission programs look like. It's awesome. However
> All trajectories are pre-computed and stored in a database that contains all the 
solutions. In order to compute new trajectories to a non-existing object and/or 
if you wish to add a particular object that is not currently available, please 
contact us and we will have it included.

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

## Asterank

[asterank](https://github.com/typpo/asterank) - "Using publicly available data and scientific papers, the project evaluates the economic prospects of mining nearly 600,000 cataloged asteroids."
They are live at http://www.asterank.com/. 

# Thanks

Included in the code are the following fine pieces of software

1. [loguru](https://github.com/emilk/loguru) from Emil Ernerfeldt for the logging
1. [CLI11](https://github.com/CLIUtils/CLI11) from Henry Schreiner for the CLI 
1. [catch(2)](https://github.com/catchorg/Catch2) from Phil Nash for unit tests

The code is passed through `clang-format` and VS code is my go to editor.
