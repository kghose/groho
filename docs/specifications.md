Scope
-----

- This is more of a strategic simulation, with the idea that realistic space travel is restricted to the solar system and that voyages last days to months, rather like the sea passages of old. 
- Rules govern "crew"/AI responses to events (ships react to events)
- The rules are flexible, and can be modified during a simulation
    - Past rules are always stored, so that a simulation can always be re-run
- Effects of collisions/explosions
- Relativistic effects
- Background bodies are objects whose motions are competely predermined and can not be altered by any event in the simulation. These include planets, planetary satellites, large asteroids. Such bodies have a crude surface and atmosphere model that can slow other objects down when they get in range, or register collisions/landings. They can also have surface locations like launch pads, landing pads, batteries. 


Break-points
------------
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (one of which is set whenever a simulation is started) and more advanced conditions trigger based on simulation state (like proximity between simulation objects, a particular object achieving a particular speed etc.)

Simulation stop points cause the current state to be saved so that the simulation can be restarted later. An existing smulation can be restarted from any one of these points. In case the restart point is not the final simulation-stop trigger, a new simulation file is saved, acting as a new branch of the simulation. This allows us to try out multiple simulation scenarios at different points.


Simulation file
---------------
Ideally we would like to be able to add and remove objects from the simulation at any time. We would like to be able to efficiently track a given object even for long simulations with many objects. We would like to be able to fork a simulation file efficiently (start a new branch of the simulation from a stop point) and keep track of all the forks. 


Command line interface
----------------------




Structure
---------

Basic engine coding:

1. Background objects

    These are planets, moons, large asteroids that are indestructible and which have stable orbits over the time scales we consider.



2. Foreground objects

These are any objects whose motions can be perturbed

2a. Passive objects. These are objects that do not move under their own power like smaller asteroids and space junk

2a. Active objects. These are objects that can sometimes move under their own power, like spaceships.

3. Attached objects



3. Solar system state. This consists of

4. Solar system history. This is a series of solar system states. The idea is to save the minimum number of states required to reconstruct the entire simulation from start to
the present. This happens in two cases



Planetary motion simulator - planets and 


Simulation file format
----------------------


