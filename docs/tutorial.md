Tutorial (v0.0)
===============

Features and use cases
----------------------

This simulator is designed to help gain intuitions of how near-future space 
flight within the solar system would look like. To this end it allows us to:

- Simulate the flights and interactions of hundreds of spacecraft
- Simulate multi-year journeys in reasonable CPU time.
- Super-impose multiple scenarios together to make a complex simulation from simpler ones
- Perform reproducible simulations (seeded random number generators, stable numerical calculations)
- Compare multiple versions of a simulation

### This is not an interactive simulation

The simulation works by setting up a scenario and then letting everything evolve 
according to physical law and scripted events. The experimenter influences the 
simulation only through the choice of scenario and script parameters.

### This is not an n-body simulation

The motions of planets, moons and larger asteroids are taken from existing 
ephemeris and are not propagated via n-body gravitational simulations. The 
expected time scales of the simulations (upto a century) are short enough that 
existing ephemeres will do fine and will save us a lot of computation. 
The gravitational effect of each planet, moon and asteroid on a ship is taken 
into account.

### Relativistic effects

Communications over solar-system distances are interestingly affected by
the finite speed of light. The simulation enables the calculation of when an event 
at one location is detected at other locations/ships.

Example scenarios
-----------------

### Earth-to-Mars Hohmann transfer

https://en.wikipedia.org/wiki/Hohmann_transfer_orbit

Starting with a spaceship in Earth orbit, we will develop a flight plan that
puts it into a Hohmann transfer and then inserts it into Mars orbit. As we do
this we'll introduce the use of a file versioning system - git in this case -
to version our simulations and overlay the different versions to figure out
a suitable flight plan. 


### Intercept of Asteroid-belt-to-Mars slow boat by Earth-launched torchship

Ship A, a slow boat (ship with limited fuel and acceleration that falls most of it's
trip) is transfering from the asteroid belt to Mars. We want to intercept the
slow boat using Ship B, a torch ship (a ship capable of sustained acceleration) 
launched from Earth. 

- First we'll develop the Ship A flight plan, de-orbiting and falling to mars transfer
- We'll then add the Ship B flight plan into the mix and then introduce the idea of
  a trigger: an event logger that tells us when something happens and can also
  cause something to happen. (Yes, perhaps this makes `groho` Turing complete)


### Fact checking "Saturn's Children"

In "Saturn's Children", Icarus (the deep-space ship) states that a direct
burn-and-decelerate would take them 18 years to get from Callisto to Eris, 
while a trip from Jupiter to Eris is almost a straight line trip because 
Jupiter and Eris are in opposition and takes 4 years (with the Mercury flyby
taking 18 mo). The other piece of information is that the exit from Callisto
is a 60G burn

This sounds hokey to me, like Stross completely made this up, but it will be
fun to pick a time when Callisto and Eris are in opposition and then work out
the different flight paths.


Simulation files
----------------
Simulations are described using text files. A **scenario file** sets up the
solar system model (The **orrery**) various simulation parameters, such as
time-step and carries a list of **fight plans** each of which is a text file
that describes the behavior of a spacecraft in the simulation.

Simulation files can be edited with your favorite editor and versioned with
your favourite versioning system. `groho` is independent of all this. What
`groho` does is monitor changes to the simulation files on the file system and 
updates the simulation computations when the files change.


User Interface
--------------

### Orbit diagram
The orbit diagram shows solar system body orbits, space craft trajectories, 
space craft status and event locations. The display can be navigated to view 
from different angles, to use different bodies as centers of reference and to 
playback or show different parts of the simulation. Events can be inspected 
to show greater detail.

### Timeline
The timeline allows us to pick parts of the simulation to show and also marks
out events so we can easily locate them in time.

### Overlaying simulations
Simulations that have the same orrery and simulation parameters can be overlaid
in the orbit display. This is useful when developing and combining flight plans.
For example, editing an existing flight plan or adding a new flight plan to a
scenario gives us the option to overlay the changed (or new) space craft trajectory
to the old one. 


Co-ordinate system re-referencing
---------------------------------
Internally the simulation is referenced to a heliocentric, cartesian reference 
frame. When we display data we translate all trajectories relative to the
camera target. 

Say we are lauching a ship into Earth orbit. The helio-centric 
trajectory of the ship is initially parallel to that of the Earth before 
breaking into epicycles that form a spiral around the Earth's path (much like
the Moon's helio-centric trajectory). 

This can be a little hard to interpret.
If we move the camera target to be the Earth, then all trajectories are 
translated - at each time point - relative to the Earth. This means the Earth's
trajectory disappears to be a point at the origin, the Moon's trajectory 
becomes a circle round the Earth and the ship's trajectory becomes recognizable
as a parabola morphing into an ellipse around the Earth.

_Note to self: If we do adaptive downsampling and never save the original points
this translation becomes tricky: we can use interpolation but now do we have
to worry about cumulative errors?_


Scenario file
=============
When you start groho, you pass a scenario file, like so

`groho earth-mars.txt`

Here `earth-mars.txt` is a scenario file, and it tells Groho what Orrery model to use (The Orrery is the thing that simulates the solar system), what range of dates to run the simulation for and what flight plan files to include in the simulation. Flight plans are the biggest part of the simulation. Each spaceship has it's own flight plan which governs what it does in the simulation.

`earth-mars.txt`, for example, starts something like this:

```
name: Earth to mars
description: Tutorial example showing how to build an Earth to mars transfer
orrery: simplesolarsystem
start: 2458061.5
stop: 2458161.5
step: 0.00001
flightplan: durga.txt
```

`orrery` can be `simplesolarsystem` which is what I use for debugging purposes, or, more typically, a JPL ephemeris kernel in `.bsp` format, like `de430.bsp`. You can have more than one `.bsp` file, by simply repeating the `orrery` line. 

`start`, `stop` and `step` are Julian Dates. The Navy has an useful [converter][jdc] to go between regular dates and Julian Dates.

[jdc]: http://aa.usno.navy.mil/data/docs/JulianDate.php

`flightplan` is a flight plan text file. You can have as many flight plans as you want, one to a line. The name of each spaceship (declared in the flightplan file) has to be unique. 

Typically you will have the scenario file and flight plan files for a simulation in one folder with different folders for different simulations, to keep organized, while the JPL ephemeris kernels are kept in one fixed data directory. 

[Flightplan][fp]
==========
For a detailed description of the flightplan file see [here][fp].

[fp]: flight-plan.md