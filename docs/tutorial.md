Tutorial (v0.0)
===============

The tutorial will present a few scenarios and then work through them.

Scenarios
=========

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
slow boat using Ship B, a torch ship launched from Earth. 

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
Groho's main window gives you a view of the simulation which you can navigate with the mouse and keyboard. The simulation is governed by scenario and flight plan files, which are ordinary text files you can create and edit with your favorite text editor. When any of the simulation files are altered the simulation is re-run automatically and the display updates. 

Controls
--------
Mouse actions govern most of the important navigation options in the display.
We control a chase camera that we can orbit around a target and zoom in and out.
The default view has the solar system center as target, and sits perpendicular
to the ecliptic.

 
| Action/Key       |       Effect                         |
|------------------|--------------------------------------|
| Mouse drag       | Orbit camera                         |
| Mouse scroll     | Move forward/backward in sim time    |
| CMD + scroll     | Move in and out of simulation        |
| [ and ]          | Change camera FOV                    | 
| Cursor keys      | Switch target to adjacent object     |
| Home             | Go to default view                   |

* _At the start I had the camera act like a first-person-shooter camera. But then
I found the current way better for what I wanted to get out of the visualization_

* The Home key on macOS is Fn + Left Cursor

Display
-------
Initially the display shows the whole simulation (advancing as the simulation
computation progresses). At any point we can move backwards/forwards in 
simulation time. Orrery objects paths are shown to one complete orbit round the
sun while spacecraft paths are shown from start to finish. This means that 
planetary paths look "normal" (like circles) but lunar paths resemble epicycles.

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

`flightplan` is a flight plan text file. You can have as many flight plans as you want, one to a line. The names of each spaceship has to be unique. 

Typically you will have the scenario file and flight plan files for a simulation in one folder with different folders for different simulations, to keep organized, while the JPL ephemeris kernels are kept in one fixed data directory. 

[Flightplan][fp]
==========
For a detailed description of the flightplan file see [here][fp].

[fp]: flight-plan.md