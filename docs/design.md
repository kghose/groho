
Look here for [user story and use case](usecase.md)


# Memoization and flight plans

A key part of enabling the above user flow efficiently is memoization (reusing
computations). We can reuse a spacecraft trajectory for a section for which the
initial state and all artificial accelerations are the same. The initial state
is easy to check. The artificial accelerations are determined by all flight
plans that are active during that section.  

We won't enable memoization in the first version, but it is advantageous to
think of how we can make the later addition of memoization easier.

## Some considerations for flight plan actions

1. The only thing a flight plan can do at each step is add an acceleration to
   the ship.
1. Each ship only has one flight action active at a time (at most). An action is
   triggered, it runs and then is discarded. 
1. Actions are invoked in the order they are present in the 

1. Flight plans are allowed to have arbitrary state. This state is saved
   periodically at checkpoints. This limits the granularity of our work re-use.
   We have to restart the simulation from the most recent checkpoint, rather
   than at an arbitrary point (which we could do if flight plans had no internal
   state). The gain in power for flightplans from this trade-off should be
   favorable.
1. Flight plans can depend on the state of other ships. This will allow us to
   simulate rendezvous, for example. For memoization this means that the two
   trajectories are now ganged together, and recomputing one, requires the other
   to be recomputed from the same time onwards, depending on the dependency.
   This makes memoization more complex and challenging.
1. Each flight plan action, including ones that are trigger based, rather than
   time based, have an activation time, which means they can not activate
   earlier than that time. This allows us to incrementally alter (add/remove)
   trigger based  actions (such as orbit insertion burns) to a flight plan and
   reuse previous work. Since the engine knows that an altered action will not
   affect the trajectory before the activation time, it will reuse the
   previously computed trajectory. A smarter engine will be able to re-run
   just the altered actions and only reengage the full simulation when any of
   them activate. This is more complex.
1. Each flight plan action marks at a checkpoint if it is active i.e. it's state
   is being actively updated and/or it is affecting the ship's acceleration.
   This allows more efficient memoization. For example, if a flight plan action
   is designed to insert a ship into orbit around a body, the flight plan does
   not become active 



# Simulation output

## Volume estimate

Assuming a simulation step of 1s, for each simulated body for a 1y simulation,
we have: 

1y * 365 day/y * 24 h/d * 60 m/h * 60 s/m * 3 points/s = 94.6 million points
~ 756 MB/object/y


## Chebyshev compression

After looking at time series databases and my own custom down-sampling I decided
to try compression by fitting to Chebyshev polynomials, same as how NASA stores
the SPK data.

See [chebyshev-file-format.md]


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

