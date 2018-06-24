Backlog
=======
(Just an unprioritized list of ideas and things to work on)

Core
----
* Display buffer data should include time so that we can scroll to or limit display by time


Simulation
----------
* Leap-frog integration
* Orrery velocity computer should be more sophisticated - do midpoints?

Display
-------
* Work on UI specifications (this is possibly in an older commit)
* Unobtrusive grid for orientation and scale
* re-framing and re-centering

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
-----------
* How do we interact - keystrokes, UI widgets, separate dialogs, command line?
* Allow selection of a long going operation (e.g. parking) and show what the distance is live
  - more generally, for each action mark out some state variables as debug variables that
    we can show live when asked for. There will be a performance hit for this ...


Logging
-------
* On screen messages or separate log file to indicate what's going on 



Donelog
=======
(For vanity)

Core
----
* [DONE] FlightPlanAction - warnings about deleted desctructors
* [DONE] Rename FlightPlan to Ship
* [DONE] File extensions should be .groho.txt for syntax highlighting
* [DONE] refactor flight plan actions to remove duplication of code/concerns
* [DONE] Refactor body objects to unify metadata scheme
* [DONE] Introduce space craft into the simulation
* [DONE] Figure out way to combine flight plan actions with spacecraft
* [DONE] Orrery should not reload if new range is inside old range
* [DONE] MagnumApp - setSamples is deprecated - find correct way to do this

Display
* [DONE] Chase camera that can track along the path of an object and can orbit round it

Simulation
----------
* [DONE] Initialiser that can spawn a spaceraft in orbit round a body

Input files
-----------
* [DONE] Need to improve readability by using named parameters


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