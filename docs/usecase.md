# User story

We start out in Ship1, in a 200x200 km Earth orbit. We perform an escape burn
and head for Ceres. We execute one or more mid-course correction burns. When we
arrive at Ceres, we perform an orbit insertion burn to 200x200 km. 

Once this trajectory design has been completed we start work on a second
trajectory for Ship2. We start in an 50x50 km orbit around Vesta, and use an ion
engine to break free and head for Ceres. We insert into a 200x200 km orbit
around Ceres. We then perform a rendezvous maneuver to dock Ship2 with Ship1
(bring it to within 100m of Ship1 with a relative speed of < 1m/s)

At each phase we want to efficiently play around with different maneuvers and
plot them as we do.


# Use case

1. Create an empty scenario file
1. Start `sim-groho`, passing this scenario file and the name of an output
   folder as command line arguments.
   > `sim-groho` creates the output folder if it doesn't exist. If it does exist
   `sim-groho` indicates this to the user, compares the cached scenario file(s)
   in this folder to the current one and figures out what, if anything, can be
   memoized. Currently the simulation is completely empty.
1. Create an orrery file.
1. Add in the DE425 `.bsp` file (basic solar system components)
1. Separately start `vis-groho` passing the plot file, the `sim-groho` output
   folder and a `vis-groho` plot folder as command line arguments. 
   > The simulation is completely empty, the plot file is non-existent, so
   > nothing is plotted
1. Include the orrery file in the scenario file
   > `sim-groho` detects a change in one of the scenario files and retries
   > simulation. Because no time limits have been set, nothing happens.
1. Indicate a start date for the simulation
   > `sim-groho` retries the simulation . No valid time limits have been set, nothing happens
1. Indicate an end date for the simulation in the scenario file
   > `sim-groho` retries and executes a simulation with the given
    scenario, which is that of all the planets for that given time range.
   
   > `vis-groho` detects a change in the output directory and tries to plot the
    data. Since the vis file is empty or non-existent a default plot is
    constructed. This is a multi-view, first-angle projection with the top view
    corresponding to the ecliptic plane.
1. Add in the `.bsp` file for ceres and vesta
  > `sim-groho` reruns and `vis-groho` replots
1. Create a flight plan file for Ship1. Add metadata about the ship
1. Add the flight plan to the scenario file
  > Since the plan is empty, `sim-groho` produces the earlier result
1. Add an initial condition to the plan, placing the ship in orbit around the
   Earth. 
   > `sim-groho` simulates the trajectory of this ship for the duration of the
   simulation
1. Add a prograde burn to send the ship towards Ceres. User is likely to
   experiment with different departure times. User can modify the orrery file to
   only include certain objects for a simulation during this experimental phase.
   For example user may choose to include only the SSB and the Earth. This
   provides a coarser, but faster simulation.
   > `sim-groho` repeatedly recomputes the trajectory, re-using computation where it can.
1. Add a line to the vis file adding an ecliptic ("top-view") plot centered on
   Ceres, showing the neighborhood as we attempt to tune our trajectory to pass
   close enough, so we can engage the orbit injection program
1. Make sure simulation is run with full Orrery so we can be assured of a
   precise trajectory.
1. Add an orbit injection program to the flight plan, allowing Ship1 to be
   captured by Ceres.
1. Add a circularization program to adjust the orbit to 200x200 km
1. Now repeat process with second ship, Ship2
1. Once both ships are in orbit, add a rendezvous program.
   > The rendezvous program itself invokes several subprograms: adjust orbital
   plane to match target, lower orbit to chase target, raise orbit to match target. 
