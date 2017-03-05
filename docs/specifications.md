Scope
-----

- This is more of a strategic simulation, with the idea that realistic space travel is restricted to the solar system and that voyages last days to months, rather like the sea passages of old. 
- Rules govern "crew"/AI responses to events (ships react to events)
- The rules are flexible, and can be modified during a simulation
    - Past rules are always stored, so that a simulation can awlays be re-run
- Effects of collisions/explosions
- Relativistic effects


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


