Tutorial (v0.0)
===============

Scenarios
=========
We'll start by considering a few space-flight scenarios 

Earth-to-Mars transfer
----------------------
We will develop a flight plan for an Earth-to-Mars transfer using a torch-ship
(spaceship capable of sustained acceleration). We will develop the plan in 
stages:
- Launch out of Earth's gravity well
- Intercept Mars
- Mars orbit insertion

Intercept of Asteroid-belt-to-Mars slow boat by Earth-launched torchship
------------------------------------------------------------------------
Ship A, a slow boat (ship with limited fuel and acceleration that falls most of it's
trip) is transfering from the asteroid belt to Mars. We want to intercept the
slow boat using Shib B, a torch ship launched from Earth. 

- First we'll develop the Ship A flight plan, de-orbiting and falling to mars transfer
- We'll then copy over the flight plan we developed earlier and add an interaction event and tweak it until the interaction event is satisfied.

Fact checking "Saturn's Children"
---------------------------------
In Saturn's Children Icarus (the deep-space ship) states that a direct
burn-and-decelerate would take them 18 years to get from Callisto to Eris, 
while a trip from Jupiter to Eris is almost a straight line trip because 
Jupiter and Eris are in opposition and takes 4 years (with the Mercury flyby
taking 18 mo). The other piece of information is that the exit from Callisto
is a 60G burn

This sounds hokey to me, like Stross completely made this up, but it will be
fun to pick a time when Callisto and Eris are in opposition and then work out
the different flight paths.


Ok, how do we do this?

User Interface
==============
Groho's main window gives you a view of the simulation which you can rotate, zoom and translate with the mouse. The simulation is governed by scenario and fligh plan files, which are ordinary text files you can create and edit with your favorite text editor. When any of the simulation files are altered the simulation is re-run automatically and the display updates. 

Controls
--------
Dragging with the mouse points the camera in different directions. The scroll wheel moves the camera forwards and backwards along the direction it is pointing. When the camera gets close to an object it's motion slows down. Holding the command key and scrolling changes the field of view of the camera. There are hotkeys that move the camera to some standard positions. The default hot keys are

| Key       |        View                         |
|-----------|-------------------------------------|
| e         | Ecliptic at 40 AU                   |
| p         | Polar at 40 AU                      |
| 1 ... 9   | Go to the vicinity of Mercury, Venus, Earth, ... Pluto and look towards the sun |

Scenario file
=============
When you start groho, you pass a scenario file, like so

`groho earth-mars.txt`

Here `earth-mars.txt` is a scenario file, and it tells Groho what Orrery model to use (The Orrery is the thing that simulates the solar system), what range of dates to run the simulation for and what flight plan files to include in the simulation. Flight plans are the biggest part of the simulation. Each spaceship has it's own flight plan which governs what it does in the simulation.

`earth-mars.txt`, for example starts like this:

```
name: Earth to mars
description: Tutorial example showing how to build an Earth to mars transfer
orrery: simplesolarsystem
start: 2458061.5
stop: 2458161.5
flightplan: durga.txt
```

`orrery` can be `simplesolarsystem` which is what I use for debugging purposes, or, more typically, a JPL ephemeris kernel in `.bsp` format, like `de430.bsp`. You can have more than one `.bsp` file, by simply repeating the `orrery` line.

`start` and `stop` are Julian Dates. The Navy has an useful [converter][jdc] to go between regular dates and Julian Dates.

[jdc]: http://aa.usno.navy.mil/data/docs/JulianDate.php

`flightplan` is a flight plan text file. You can have as many flight plans as you want, one to a line. Ideally, the names of each spaceship should be unique. Clashing names will be renamed internally with suffixes like `II`, `III` but this may break any ship-ship interaction actions, so basically, name each ship uniquely. 

[Flightplan][fp]
==========
For a detailed description of the flightplan file see [here][fp].

[fp]: flight-plan.md