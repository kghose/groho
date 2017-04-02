Scope
=====

- This is not an interactive simulation
- A major aspect is the AI which will plan and execute ship trajectories and respond to events
- The simulation space is restricted to the solar system
- Motions of planets, moons and larger asteroids are taken from existing ephemeris and are not propagated via n-body simulation
- Relativistic effects will be simulated


Simulation
==========

Parts
-----
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (which is set whenever a simulation is started) and more advanced triggers based on simulation state (like proximity between simulation objects, a particular object achieving a particular state etc.) are available. One such stretch of a simulation is called a part. 

Forking
-------
A simulation can be restarted from any point within an epoch. This constitues a "fork" of the simulation and we can use it to test out different scenarios, different AI models etc. A complete simulation then resembles a tree consisting of a connected parts. It is possible to manage this tree quite simply. 



- This is not a real-time simulation
    - There is no user interactivity
    - AI modules cause Ships to respond to events and conditions
    - The simulation runs until a stop event is reached
    - Simulations can "forked" to test out different scenarios, different AI models etc. 

- The universe is restricted to the solar system.
    - The idea is that realistic space travel is restricted to the solar system and that voyages last days to months, rather like the sea passages of old.
    - There are ships with high constant acceleration ("torchships") which permit travel to nearby stars, however, we don't handle that here.
- A major aspect is the AI which will plan and execute trajectories and respond to events
    - We can modify a ship AI after a simulation stops and then restart the simulation.
- Motions of planets, moons and larger asteroids are taken from existing ephemeris and are not propagated via n-body simulation
- Relativistic effects will be simulated


        - The simulation can be restarted after a stop.
        - It is easy to 'fork' a simulation at a stop point, so simulations end up looking like a tree, with each branch point being an alternative scenario that we test, usually by changing the AI or AI goals and restarting after a stop



Components
==========

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
------------
Spawn points are attached to spots or ships. They serve as origins and destinations for ships allowing us to add ships at any time during the simulation. 


Simulation
==========

- A cartesian coordinate system centered on the solar system barycenter is used.
- [Orbital simulations](orbits.md)


Object interaction models
-------------------------


Break-points
------------
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (one of which is set whenever a simulation is started) and more advanced conditions trigger based on simulation state (like proximity between simulation objects, a particular object achieving a particular state etc.)

Simulation stop points cause the current state to be saved so that the simulation can be restarted later. An existing smulation can be restarted from any one of these points. In case the restart point is not the final simulation-stop trigger, a new simulation file is saved, acting as a new branch of the simulation. This allows us to try out multiple simulation scenarios at different points.

Simulation forking
------------------
Ideally we would like to be able to add and remove objects from the simulation at any time. We would like to be able to efficiently track a given object even for long simulations with many objects. We would like to be able to fork a simulation file efficiently (start a new branch of the simulation from a stop point) and keep track of all the forks. 




Export triggers and models
--------------------------


Simulation


Command line interface
======================


