Specifications: overview
========================

- Simulator - runs in its own thread computing the simulation
- Display   - runs in its own thread, reads simulation to provide visualization

Simulator
---------
- Reads simulation file, generates simulation
- Simulation interrupted and restarted when simulation file changed


Display
-------
- Stores simulation data for last N simulations
  - Only if orrery model is the same
- Can visualize any one of the N simulations or superpositions of any subset of them
- Simulations are refrerenced by hash of simulation file

Simulation file
---------------
- Set of text files, one "main" file and a set of "flight plans" 
- Main file carries orrery description, simulation time and time step, and list of flight plans
- Flight plans carry description of spacecraft followed by list of actions/triggers


Simulation Behaviors
--------------------
The solarsystem is fixed, and does what it does based solely on the current time. 
Spaceships move in response to gravitational attraction and their own engines.
Spaceships can appear (Spawn) and disappear (Exit) at any time. They can stop 
falling and become embedded in a solar system body (Land/Crash). They can start 
falling again (Launch). A spaceship's engine acceleration can change in arbitrary
ways and at abitrary times. One spaceship may influence another in an arbitrary
way.


Saved (serialized) data
-----------------------
No plans to serialize simulation data at the moment


Display
-------
The display enables interactive adjustment of views of the simulation, including
centering on different bodies, different camera views and tracking and panning.
A static view can be exported to pdf with labels etc.