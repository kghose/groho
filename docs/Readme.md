# Developer documentation

## Major classes
![](uml/png/classes.png)

*It may turn out that Flightplans need state.

## High level activity diagrams

### (Re)starting a simulation
![](uml/png/startup.png)

An important part of making the program more responsive to interactive
trajectory development is to only recompute when it is necessary. We do this by
maintaining periodic checkpoints of the simulation and analyzing changed
scenario files to see if we can restart from an existing checkpoint, rather than
having to start from the begining.

### Central simulator loop
![](uml/png/simulatorloop.png)

### Serialization threads
![](uml/png/serialize.png)


## Sequence diagram

![](uml/png/sequence.png)

While the sequence diagram shows the simulation restarted every time the
scenario file is edited, internally we look for opportunities to
cache and reuse computation. So depending on if and how the scenario file
changes the `restart` command may mean the simulation just carries on, or it
restarts from a previous checkpoint in the simulation. Similarly for the
plotting, if the data is unchanged, or if the plotting file is unchanged, the
plot is not redone.


## Plotter loop
![](uml/png/plotterloop.png)

## Saving data

We will find many fascinating ramblings [here](dev). The one sensible thing I
did was invent the Fractal Downsampler. [Here is a notebook describing
it](dev/adaptive-display-points.ipynb) and a [blog
post](https://kaushikghose.wordpress.com/2017/11/25/adaptively-downsampling-a-curve/).

I considered two ways of saving the data: A) write out each trajectory to a
different file or B) interleave the trajectories in the same file, using a one
or two byte tag to indicate which trajectory each data point belonged to. 

A. creates a bunch of files, but is easier to process for display. B. Is more
compact, but requires more complex pre-processing before display. When in doubt
do the simpler thing. So the simulator writes out data into a directory with one
file per trajectory.

Simulated data at each step is passed into the fractal downsampler. When the
downsampler says it's time to save a sample, the sample is written out. Double
buffering is used.

### Folder organization
The folder is populated with the following files 
- `posX.bin` where `X` is the NAIF id of the object.
- `bodies.txt` a manifest file listing all the bodies in the simulation
- `events.txt` a list of events and their times


# [Current road map](roadmap.md)