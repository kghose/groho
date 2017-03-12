Scope
=====

- This is more of a strategic simulation, with the idea that realistic space travel is restricted to the solar system and that voyages last days to months, rather like the sea passages of old. 
- Rules govern "crew"/"AI" responses to events (ships react to events and have AI). The rules are flexible, and can be modified during a simulation
- Effects of collisions/explosions
- Relativistic effects


Simulation fidelity
===================

I'm using NASA's open source and gorily documented [GMAT][gmat] software as the gold standard for solar system simulation. A quick peruse of their [mathematical specifications document][gmat-spec], found in [their contributors page][gmat-contrib] shows how complicated a proper simulation is, and requires several years worth of background. The first chapter, for example, is solely on the topic of time and how to measure it.


[gmat]: http://gmatcentral.org/
[gmat-spec]: http://sourceforge.net/p/gmat/code/HEAD/tree/trunk/doc/SystemDocs/MathematicalSpecification/GMATMathSpec.pdf?format=raw
[gmat-contrib]: http://gmatcentral.org/display/GW/For+Contributors

My attempt here will be to start with rather simple models of solar system motion - starting with keplerian orbits for the planets and other immutable objects - but design the program so that more complex models can be easily swapped in as my understanding improves.


Components
==========

Signals
-------
These represent electromagnetic emissions (radio waves, laser pulses, x-rays) that can carry information. They obey the inverse square law have a directionality profile, have a duration, and propagate at the speed of light. 


Immutable objects
-----------------
These are planets, moons and large asteroids whose motions are competely predermined and can not be altered by any event in the simulation over the time scales we consider (up to a century). Such bodies have a crude surface and atmosphere model that can slow other objects down when they get in range, or register collisions/landings. These objects supply the gravitational attraction that affects trajectories of other objects.


Fixed objects
-------------
These model things like cities, launch pads, landing strips and batteries that have almost the same properties as foreground objects, except that they are immobile on the surface of an immutable object. They expose a position and orientation vector parameter which the simulation uses to compute their location in solar cooridnates.

Foreground objects
------------------
These are objects whose motions are affected by gravitational attraction from background objects, by interaction with background and foreground objects (collisions, explosions) and by engine propulsion. Foreground objects are the most sophisticated elements of the simulation as they can react to events. 

The object is visible to the simulation as a point mass with an attached force vector. The force vector describes the resultant force on the object from non-gravitational sources, like engines, out-gassing etc. These two components are used by the simulation to update the position and velocity of the object. 

In turn, signals, background objects and other foreground objects are visible to the object. The object's interaction model determines how the object responds to them. 

Spawn points
------------
Spawn points have to attached to either fixed objects or foreground objects. They serve as origins and destinations for simulation objects.   



Simulation
==========

Coordinate system
-----------------
A cartesian cooridnate system centerd on the sun (the abrycenter is assumed to ) is used



Object interaction models
-------------------------


Break-points
------------
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (one of which is set whenever a simulation is started) and more advanced conditions trigger based on simulation state (like proximity between simulation objects, a particular object achieving a particular speed etc.)

Simulation stop points cause the current state to be saved so that the simulation can be restarted later. An existing smulation can be restarted from any one of these points. In case the restart point is not the final simulation-stop trigger, a new simulation file is saved, acting as a new branch of the simulation. This allows us to try out multiple simulation scenarios at different points.


Simulation file (Serialization format)
======================================
Ideally we would like to be able to add and remove objects from the simulation at any time. We would like to be able to efficiently track a given object even for long simulations with many objects. We would like to be able to fork a simulation file efficiently (start a new branch of the simulation from a stop point) and keep track of all the forks. 

Export triggers and models
--------------------------



Command line interface
======================




