# Targeted user flow/use cases
1. Setup basic scenario. 
1. Add one spaceship and develop trajectory incrementally
  - Develop one part of trajectory in detail. Inspect trajectory, plot it,
    compute metrics (like closest approach to target etc.)
  - Move on to subsequent leg of trajectory, freezing previous part
1. Add next spaceship and repeat, keeping earlier spaceship (or others) unchanged
1. Print high resolution trajectory plots in different ways


# Memoization and flight plans

A key part of enabling the above user flow efficiently is memoization (reusing
computations). We can reuse a spacecraft trajectory for a section for which the
initial state and all artificial accelerations are the same. The initial state
is easy to check. The artificial accelerations are determined by all flight
plans that are active during that section.  



## Some considerations for flight plan actions

1. While it would be cool to have flight actions triggered by the state of other
   ships (via messages) it would make memoization more complex. For this reason
   we do not implement any actions that depend on anything in the simulation
   that can be changed, like trajectories of other spacecraft, or other actions.
1. Each flight plan action, including ones that are trigger based, rather than
   time based, have an activation time, which means they can not activate
   earlier than that time. This allows us to incrementally alter (add/remove)
   trigger based  actions (such as orbit insertion burns) to a flight plan and
   reuse previous work. Since the engine knows that an altered action will not
   affect the trajectory before the activation time, it will reuse the
   previously computed trajectory. A smarter engine will be able to re-run
   just the altered actions and only reengage the full simulation when any of
   them activate. This is more complex.


# User interface

1. Simulations are described using text files.
1. The entry point file is called the Scenario file. It contains some global
   metadata about the simulation (such as start and stop times) and a list of
   other simulation files: an orrery file and zero or more flight plan files. 
1. The orrery file contains a list of .bsp files that comprise the solar system
   model for this simulation
1. Each flight plan file is a list of actions, their times of activation and
   parameters. 



# Rough notes
1. Developing and maintaining an interactive visualization component in C++ is
   taking too much time away from the core of the program. 
1. In the end I want a high quality, printable, annotated plot. 
1. I want to plot a part of a trajectory from different viewpoints and in
   different reference frames.
1. I will want to focus on modifying specific parts of the trajectory,
1. **Allowing spacecraft to interact means that we can not develop trajectories independently and incrementally (or at least it will be very complex).**



# Compute engine milestones

1. Read simulation file and produce simulation saved in single 
   data file using single thread
2. Use multiple threads to parallelize orrery computations 
   (to compute net acceleration vector for each ship)
3. Do incremental computation (use memoization) when
   a) more ships are added to a simulation
   b) end time of a simulation is extended   
4. Use spline interpolation (or other method) to reduce output data size
5. Use a coarse engine (patched conics?) to allow faster iteration of
   trajectory design. 


# Flight plan/scenario engine milestones

1. 



# Viz engine milestones

1. Non-interactive PDF output that updates as simulation data is updated
   a) Displays more data as more data is computed
   b) Refreshes plot if simulation is restarted
   c) Refreshes plot if display parameters are changed

2. Smart, adaptive plot that only shows trajectories that are spaced far 
   enough apart (avoids visual clutter). Also displays events.

