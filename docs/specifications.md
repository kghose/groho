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

Simulation branching
--------------------
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (one of which is set whenever a simulation is started) and more advanced conditions trigger based on simulation state (like proximity between simulation objects, a particular object achieving a particular state etc.) An existing smulation can be restarted from any point. This starts a new branch of the simulation. A set of 


In case the restart point is not the final simulation-stop trigger, a new simulation file is saved, acting as a new branch of the simulation. This allows us to try out multiple simulation scenarios at different points.


Training simulations
--------------------
The AI is trained on one or more scenarios repeatedly until it achieves a satisfactory performance. It is possible to run the training sessions in parallel, synching up the AI from different parallel instances when needed. Diagnostics from the simulations can be saved to be inspected later.


Test simulations
----------------
These are typically longer and more elaborate than training simulations and are designed so we can organize complex scenarios into smaller, more manageable components.


Simulation forking
------------------





Ideally we would like to be able to add and remove objects from the simulation at any time. We would like to be able to efficiently track a given object even for long simulations with many objects. We would like to be able to fork a simulation file efficiently (start a new branch of the simulation from a stop point) and keep track of all the forks. 




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

