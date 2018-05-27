Backlog
=======
(Just an unprioritized list of ideas and things to work on)

Core
----
* Display buffer data should include time so that we can scroll to or limit display by time
* Leap-frog integration
* FlightPlanAction - warnings about deleted desctructors
* Rename FlightPlan to Ship
* MagnumApp - setSamples is deprecated - find correct way to do this

Display
-------
* Work on UI specifications (this is possibly in an older commit)
* Unobtrusive grid for orientation and scale
* Chase camera that can track along the path of an object and can orbit round it
* Superimpose multiple buffers
  - what to do for identical tracks? Make condition of superimposition that the
    Orrery and time range are the same - only flight plans can change (be added,
    removed, edited)
* At long ranges bodies are dots, when closer in they become meshes/solid figures
* When a spaceship is in proximity to a body, the radially projected lat/lon 
  point is shown on the body with coordinates
* For the Orrery show 0.5 orbit before and 0.5 orbit after the current time
  Otherwise, we get lost in the solar system when 

Interaction
* How do we interact - keystrokes, UI widgets, separate dialogs, command line?

Logging
-------
* On screen messages or separate log file to indicate what's going on 



Donelog
=======
(For vanity)

Core
----
* Refactor body objects to unify metadata scheme
* Introduce space craft into the simulation
* Figure out way to combine flight plan actions with spacecraft
* Orrery should not reload if new range is inside old range


Version planning
================

v 0.0.0
-------
- Approximate position formulae (from E M Standish)
	- How to handle Earth-Moon orbits given Barycenter?
- Orrery model should be pluggable (so we can swap in model for v1.0.0 easily)
- Visualization code should be functionally 


- All bodies are spherical for collision detection
- Scripted actions
- Think about visualization


Learning for
- Numerical Integration
- Scripted actions
- Simulation save files
- Visualization