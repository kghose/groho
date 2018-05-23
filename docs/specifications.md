Deprecated
=========

Scripting (v0.0)
=========

In my dreams the space craft control is a very elaborate apparatus going all 
the way up to AI controlled spacecraft. In reality, for the forseeable future, 
spacecraft control will consist of linear scripts that indicate timed 
engine and attitude state changes. 




The interface should make it simple to fork a simulation 

Spacecraft AI can be trained, but they will have to operate in batch fashion - executing scenarios


that is programmed will have to work in batch fashion - 


Simulation interaction (v0.0)
==============
Though nominally a non-interactive simulation there should be a mouse + keyboard
interface that allows us to

- Scroll backwards and forwards in time along a simulation
- Select particular trajectories and objects to view
- Fork simulations
  - Change spacecraft scripts



Spacecraft models (v0.0)
=================





Use cases
=========

Transfer orbits
---------------
I want to understand GTOs and how satellites are launched first into a GTO and then raise themselves to a GSO using a kick motor. This is restricted to scenarios with one planet and may be the moon. In-homogeneities, which cause orbit precession, like the equatorial bulge, may need to be modeled. 

Multi-planet transfers
----------------------
Scenarios where we launch a spaceship and then repeatedly fire engines to jump from one orbit to another. 

Solar-system freeway
--------------------
Simulations at time scales long enough to simulate multi-year journeys across the solar system, including the "solar system freeway" that allows a spaceship to travel to the outer reaches of the solar system using very little fuel.

Behavior of debris fields when spaceships collide
-------------------------------------------------
What kind of orbits do the debris fields follow? How does the debris spread? How long do collisions on "highways" between planets stay dangerous.



Requirements/features
=====================
- Simulate multi-year journeys in reasonable time.
- Compose multiple scenarios together to make a complex simulation from simpler ones
- Reproducible simulations (seeded random number generators, stable numerical calculations)
- Checkpoints



Scope
=====

This is not an interactive simulation
-------------------------------------
The simulation works by setting up a scenario and then letting everything evolve according to physical law and the reactions of the AI to events. The experimenter influences the simulation only through the choice of scenario parameters and AI programming.


AI is a major focus
-------------------
Learning algorithms that take actions ("actors") are interestingly different from learning algorithms that classify. I want to use this simulation framework to explore how one can get actors to learn how to pilot spaceships. The simulation framework is therefore built to make it easy to train AI as well as test the AI. Design requirements for train and test simulations are different.

For training we want to run a fairly simple scenario many times while the AI learns. We are not necessarily interested in the trajectory of the ships for every run, but we are interested in how well the AI is converging to a viable strategy and require diagnostics to check if our objective functions reflect the physical goal we are interested in.

For testing we want potentially much more complex scenarios that are run once. We are mainly interested in the ship trajectory and some diagnostics that give us insight into the decisions the AI made.


Branching simulations
---------------------
Another major focus is to provide an easy to use and powerful way to retry simulations from different points in time with various parameters (such as AI or scripting) changed. The simulations rerun this way form a tree with a branch point occuring whenever a simulation is started from a mid-point. We want an easy to use system to keep track of all the branches in this "what if?" tree.


This is not an n-body simulation
--------------------------------
The motions of planets, moons and larger asteroids are taken from existing ephemeris and are not propagated via n-body gravitational simulations. The expected time scales of the simulations (upto a century) are short enough that existing ephemeres will do fine and will save us a lot of computation. The gravitational effect of each planet, moon and asteroid on a ship is taken into account.


Relativistic effects will be simulated
--------------------------------------
An interesting question for me is how the finite speed of light over solar-system distances affects interactions as well as how relativistic effects come into play for constantly accelerating spaceships that achieve speeds significantly close to light speed. 



Simulation Components
=====================

Signals
-------
These represent electromagnetic emissions (radio waves, laser pulses, x-rays) that can carry information. They obey the inverse square law, have a directionality profile, have a duration, and propagate at the speed of light. 


Rocks
-----
These are planets, moons and large asteroids whose motions are competely predermined and can not be altered by any event in the simulation over the time scales we consider (up to a century). Such bodies can have a crude surface and atmosphere model that can slow other objects down when they get in range, or register collisions/landings. These objects supply the gravitational attraction that affects trajectories of Ships.


Spots
-----
These model things like cities, launch pads, landing strips and batteries that are immobile on the surface of Rocks. They expose a position and orientation vector parameter which the simulation uses to compute their location in solar cooridnates. Spots may be added or removed at any time during the simulation. Spots can have AI.


Ships
-----
These are objects whose motions are affected by gravitational attraction from rocks, by interaction with rocks, spots and other ships (collisions, explosions) and by engine propulsion. Ships have AI. 

A ship is visible to the simulation as a point mass with an attached force vector. The force vector describes the resultant force on the object from non-gravitational sources, like engines, out-gassing etc. These two components are used by the simulation to update the position and velocity of the ship. 

In turn, signals, rocks and spots are visible to the ship. The ship's interaction model determines how the ship responds to them. 


Spawns
------
Spawn points are attached to spots or ships. They serve as origins and destinations for ships allowing us to add ships at any time during the simulation. 


Simulation concepts
===================

Simulation end
--------------
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (one of which is set whenever a simulation is started) and more advanced conditions trigger based on simulation state (like proximity between simulation objects, a particular object achieving a particular state etc.) 


Simulation trees
----------------
An existing smulation can be restarted from any point with altered parameters. This starts a new branch of the simulation. A set of such branches leading to a common start node constitute a simulation tree. 

*Restarting a simulation with no changes will lead to the exact same simulation except for numerical instabilities. Care will be taken to minimize numerical instabilities*

Tools will be provided that allow us to quickly navigate such a tree, inspecting simulation state at any point in the tree, reading and writing notes for branch-points and quickly seeing what was changed at a branch point.

Simulation scenario
-------------------
This is a  




Modifying simulations
---------------------
Both the AI and the scenario can be modified




Training simulations
--------------------
The AI is trained on a scenario repeatedly until it achieves a satisfactory performance. It is possible to run the training sessions in parallel, synching up the AI from different parallel instances when needed. Diagnostics from the simulations can be saved to be inspected later.


Test simulations
----------------
These are typically longer and more elaborate than training simulations and are designed so we can organize complex scenarios into smaller, more manageable components.










Export triggers and models
--------------------------


Simulation


Command line interface
======================







Parts
-----
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (which is set whenever a simulation is started) and more advanced triggers based on simulation state (like proximity between simulation objects, a particular object achieving a particular state etc.) are available. One such stretch of a simulation is called a part. 

Forking
-------
A simulation can be restarted from any point within an epoch. This constitues a "fork" of the simulation and we can use it to test out different scenarios, different AI models etc. A complete simulation then resembles a tree consisting of a connected parts. It is possible to manage this tree quite simply. 

