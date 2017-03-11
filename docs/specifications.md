Scope
=====

- This is more of a strategic simulation, with the idea that realistic space travel is restricted to the solar system and that voyages last days to months, rather like the sea passages of old. 
- Rules govern "crew"/"AI" responses to events (ships react to events and have AI). The rules are flexible, and can be modified during a simulation
- Effects of collisions/explosions
- Relativistic effects


Detailed Scope
==============

Signals
-------
These represent electromagnetic emissions (radio waves, laser pulses, x-rays) that can carry information. They obey the inverse square law have a directionality profile, have a relatively short lifespan and propagate at the speed of light. 


Background objects
-----------------
These are planets, moons and large asteroids whose motions are competely predermined and can not be altered by any event in the simulation over the time scales we consider (up to a century). Such bodies have a crude surface and atmosphere model that can slow other objects down when they get in range, or register collisions/landings. Background objects in most cases supply gravitational attraction that affects trajectories of other objects. Some background objects (like asteriods, may be excluded from gravity computations for efficiency)

### Surface objects
Each background object can have a number of associated surface objects like launch pads, landing pads and batteries which serve as origins and destinations for simulation objects. Signals, background objects and other foreground objects are visible to a surface object's interaction model. The model can respond to these stimuli by creating or absorbing  


Foreground objects
------------------
These are objects whose motions are affected by gravitational attraction from background objects, by interaction with background and foreground objects (collisions, explosions) and by engine propulsion. 

### Object interaction models
Foreground objects are the most sophisticated elements of the simulation as they can react to events. 

The object is visible to the simulation as a point mass with an attached force vector. The force vector describes the resultant force on the object from non-gravitational sources, like engines, out-gassing etc. These two components are used by the simulation to update the position and velocity of the object. 

In turn, signals, background objects and other foreground objects are visible to the object. The object's interaction model determines how the object responds to them. 

Break-points
------------
A simulation stops when a simulation-stop trigger condition is satisfied. The basic condition is a time-end trigger (one of which is set whenever a simulation is started) and more advanced conditions trigger based on simulation state (like proximity between simulation objects, a particular object achieving a particular speed etc.)

Simulation stop points cause the current state to be saved so that the simulation can be restarted later. An existing smulation can be restarted from any one of these points. In case the restart point is not the final simulation-stop trigger, a new simulation file is saved, acting as a new branch of the simulation. This allows us to try out multiple simulation scenarios at different points.


Simulation file (Serialization format)
======================================
Ideally we would like to be able to add and remove objects from the simulation at any time. We would like to be able to efficiently track a given object even for long simulations with many objects. We would like to be able to fork a simulation file efficiently (start a new branch of the simulation from a stop point) and keep track of all the forks. 


Command line interface
======================




