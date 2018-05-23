Technical Specifications
========================
The [tutorial][tut] serves as a user level specification for the program. The
specifications here are developed based on the tutorial and serve as a guide
for when developing code.

[tut]: ../tutorial.md


Specifications: overview
========================

For the first pass we cleanly separate out the simulator core from the display.
The simulator is a standalone application that saves simulation data to disk
independently of the display application. The display application monitors a 
given data file and redraws the data as needed.

This allows us to:
- develop the two parts largely independently
- forces us to think about how we serialize data
- may force us to think about how to make serialization efficient
- allows for different display formats (like we can develop a pdf writer later)
- allows us to develop the simulator without spending time on the display 
  (we can use Python to visualize the simulations in the interim)

Drawbacks
- This can be slow in operation since it involves disk-access


Simulator
---------
- Computes simulation based on scenario file
- Simulation interrupted and restarted when any flight plan file is changed/added
- Simulation is saved to a data file tagged with a hash of the scenario
  - This hash governs internal labeling over overlaid simulations


Display (v0.0)
--------------
- Can be simple Python script
-  
- For the same scenario, can visualize any one of the last N simulations or 
  superpositions of any subset of them


Scenario file
-------------
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


[Saved (serialized) data (v0.0)](simulation-file.md)
----------------------------------------------------

Display
-------
The display enables interactive adjustment of views of the simulation, including
centering on different bodies, different camera views and tracking and panning.
A static view can be exported to pdf with labels etc.


Data transfer between simulation and display
============================================

The most efficient way would be for the simulator to write directly into the
GPU via [memory mapping][magnum-mmap]. This can get ugly and also requires
a precise order of initialization of display vs simulation, since we now require
a valid OpenGL context before starting the simulation.

[magnum-mmap]: http://doc.magnum.graphics/magnum/classMagnum_1_1GL_1_1Buffer.html#GL-Buffer-data-mapping

Until we are sure we are going to run into memory and time issues, we therefore
startout with the simplest, clearly separated design, where the simulator writes
data to it's own buffer and the display fetches this data when the simulator is
done.