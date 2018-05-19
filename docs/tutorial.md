Tutorial (v0.0)
===============

Features and use cases
----------------------

This simulator is designed to help gain intuitions of how near-future space 
flight within the solar system would look like. To this end it allows us to:

- Simulate the flights and interactions of hundreds of spacecraft
- Simulate multi-year journeys in reasonable CPU time.
- Compose multiple journeys together to make a complex simulation from simpler ones
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
solar system model (The **orrery**), various simulation parameters, such as
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
; This scenario file sets up an orrery containing the 9 planets and
; dwarf planets and the moons of mars and incorporates two flight plans

; Some notes on scenario file syntax:

; Anything from a semi-colon onward is ignored
    ; Leading and trailing whitespace is trimmed
    ; Empty lines are ignored

name  = Earth to Mars
begin = 2458248.5 ; This is in Julian Date
end   = 2458348.5 ; A 100 day simulation
step  = 60 ; This is in seconds
           ; The finer the step the more accurate the simulation
           ; but the longer it takes to run

; Both orrery and flightplan can have multiple entries

; Orrery refers to a NASA/JPL SPK file storing ephemeris data

; You can have multiple orrery files like this:
orrery = de430.bsp
orrery = mar097.bsp


; You can have multiple flightplans like this:
flightplan = durga.txt
flightplan = kali.txt
```

The Navy has an useful [converter][jdc] to go between regular dates and Julian Dates.

[jdc]: http://aa.usno.navy.mil/data/docs/JulianDate.php


Flight Plan
===========

```
; Spaceship commands are placed in a separate script file (called a "flight plan") 
; and a reference is put in the scenario file. 

; Flight plans begin with a header section that describes some ship metadata ...

name             = Durga

; some characteristics ... 

max-acceleration = 10    ; m/s^2 (a)
max-fuel         = 100   ; arbitrary units (u)
fuel-cons-rate   = 0.1   ; u / s / a 

; and some initial conditions ...

flight-state     = falling    ; Either landed or falling 
position         = 150e6 0 0  
; If falling, this is position relative to Solar System bary-center
; If landed, this is lat longitude relative to a body, and would look like
; position  = 399 38.728521 -77.251199  
; 399 being the SPK ID for earth

velocity         = 10 0 0     ; Initial velocity vector


; This is followed by a plan section which is a list of actions. 
; Each action has a time stamp followed by the name of the action and any
; arguments it takes. Like so

; <timestamp> <action name> <argument1> <argument2> ...

; The timestamp is the julian date

; An action will only happen after it's listed time. Actions happen 
; sequentially as they appear in the file. 
; It is illegal to have out of order time stamps in a flight plan. 

plan = 

2458248.5 set-attitude 1 0 0
2458248.5 set-accel    0.5
2458258.5 set-accel    0.0
2458278.5 set-attitude 0 0 1
2458278.5 set-accel    0.5
2458288.5 set-accel    0.0
```


Available actions
-----------------
_(The following actions are planned. If an action is implemented it will say implemented)_

```
Action:        cancel
Arguments:     NONE 
Description:   If there is a currently ongoing blocking action, that action is
               stopped. Otherwise, it has no effect.
```


```
Action:        interaction-event
Arguments:     spaceship s, float r, string id
Description:   If the spaceship s comes within r m of us, on or after the action
               timestamp create an event labeled with id
Notes:         This causes this flight-plan to be *soft*-dependent on the 
               flight-plan of s: If flight-plan s changes from a timestamp 
               before this, this event check will be rerun.
```


```
Action:        interaction-deactivate-event
Arguments:     spaceship s, float r, string id
Description:   If the spaceship s comes within r m of us, on or after the action
               timestamp remove us from the simulation. Effective on or after 
               time stamp of action. This creates an event labeled with id
Notes:         This causes this flight-plan to be *soft*-dependent on the 
               flight-plan of s: If flight-plan s changes from a timestamp 
               before this, this event check will be rerun and the simulation
               will be clipped at this point if needed.   
```


```
Action:        launch
Arguments:     float h, float p
Description:   If the spaceship is "landed" on a body, then it takes off 
               vertically to a height h above the surface and then orients
               tangentially and keeps thrusting till it's periapsis is at
               least p. The reference is the body the spaceship was landed on.
               Has no effect if the spaceship is not landed.
Blocking:      This action blocks
Termination:   This action terminates (stops blocking) when any of the following occurs
               1. The periapsis is achieved
               2. Fuel runs out
               3. The spaceship is land-bound (landed, crashed, waiting for take off etc.)
               4. A cancel command is encountered
```

```
Action:        set-accel
Arguments:     float l 
Description:   Set accel from engines at [0.0, 1.0] fraction of maximum accel
Status:        implemented
```

```
Action:        set-attitude
Arguments:     float x, float y, float z 
Description:   Set attitude absolute direction to (x, y, z)
Status:        implemented
```

```
Action:        rattitude
Arguments:     body b, float t, float p 
Description:   Set attitude relative to body b, at angle t degrees in 
               orbital plane, p degrees out of orbital plane
```

```
Action:        park
Arguments:     body b, float p, float a 
Description:   Park in orbit around body b with periapsis p and apoapsis a
Blocking:      This action blocks
Termination:   This action terminates (stops blocking) when any of the following occurs
               1. The periapsis and apopasis are achieved
               2. Fuel runs out
               3. The spaceship is land-bound (landed, crashed, waiting for take off etc.)
               4. A cancel command is encountered
Notes:         This parks the spacecraft in the plane it currently is in.
                
```

```
Action:        change-plane
Arguments:     TBD 
Description:   Change plane of orbit
Blocking:      This action blocks
Termination:   TBD
```

Notes:

1. Blocking commands (like park) only release control if their termination condition
   is met or a `cancel` command executes

C++ developer notes
===================

A flight plan is stored internally as a list of command objects. Each command
object takes as input the state of the simulation and produces as output
changes to the attitude and engine level of the spaceship. Multiple command
objects can be active at the same time and will be executed in the order in which
they first became active. A command object becomes active once the simulation
time crosses it's activation time. A command deactivates when it terminates.
A `cancel` command will terminate all active commands.

