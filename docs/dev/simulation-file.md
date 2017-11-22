Serializing simulations
=======================
(Serializing means saving and loading simulations from disk)

A simulation is saved as a series of checkpoints which store spacecraft
position, state and events. Orrery object positions are not stored as they are
pre-determined.

For many uses a linear (or perhaps spline) interpolation of the raw checkpoint
positions will be sufficient. In case it is needed, the simulation engine can
be used to re-simulate data between checkpoints.


Compression
===========
Plain text compresses remarkably well with xz and it's example code seems to
be easy to follow

https://github.com/xz-mirror/xz/blob/master/doc/examples/01_compress_easy.c









Data file format
================
The simulation file is a list of (time, x, y, z) triplets in resolution of 1s


Data format considerations
==========================
The use case for groho is to simulate a few spacecraft, say 10 or less, for
potentially long periods of time, say 1 year to maybe 10 years.

The solar system size is ~40 AU to the orbit of pluto, 120 AU to the oort cloud
1 AU = 150e6 km. This needs about 45 bits of representation for a 1m resolution

The simplest file format is (x, y, z) triplets coded in 64 bit, saved at fixed 
1 second intervals. For a 1 year simulation this takes 

1y * 365 day/y * 24 h/d * 60 m/h * 60 s/m * 3 * 8 bytes/s ~ 756 MB/object/year.

Now we anticipate that spacecraft trajectories will have certain patterns like,
they will sometimes be very regular, the deltas between two time steps will be
much, much smaller in magnitude than the absolute values and so on. Somewhat
complex compression schemes can be developed to exploit such patterns in the
data, but we will take a different approach, given that our data is from a 
simulator.

We anticipate that our use for the spacecraft trajectories will be to plot
them and inspect them, often at very zoomed out scales, and to inspect
space craft positions at different points in time.

Assuming that the simulation itself will be reasonably fast, since we are 
propagating the positions of a few spacecraft affected by a few tens of bodies,
instead of saving the raw trajectories continuously, we can save checkpoints,
say every hour or every day of simulation time, reducing our data size
by several orders of magnitude. Say we save a checkpoint every simulation day
we then have:

1y * 365 day/y * 3 * 8 bytes/day ~ 8.7 kB/object/year

and 

1y * 365 day/y * 24 h/day * 3 * 8 bytes/h ~ 210 kB/object/year

which are both quite compact.

When we visualize the data we will propagate the simulation between checkpoints
which can be done in parallel for multiple checkpoints and objects. 
At zoomed out scales it may even be sufficient to create simple interpolations 
between checkpoints.

In terms of data compression a slightly smarter scheme would be to save 
checkpoints based on the jerk, or rate of change of acceleration, corresponding 
to the heuristic that fewer checkpoints work when not much is going on 
(e.g. interplanetary transit) and more checkpoints are needed when something 
happens (change in burn, change in  attitude, close encounter with gravitational body etc.)

However, this places a greater burden on the reconstruction of the trajectories
and so it is better to use a metric such as total number of computational steps
between checkpoints, which allows us more controlled reconstruction during
display.

Spaceships can appear and disappear at anytime 

What about the orrery which can be recomputed at any time? Rotation of the planets?

- to simplify our life, let us assume that simulation playback/inspection will
  take place in Groho itself and we'll have access to the Orrery, so we can 
  reconstruct planetary positions and orientations


References
==========
Though we are not going to use the ideas here, this paper from the field
of molecular dynamics simulations nicely enumerates the ways such trajectories
can be compressed - [Huwald 2016][molec] "Compressing molecular dynamics 
trajectories: breaking the one-bit-per-sample barrier"

[molec]: https://arxiv.org/abs/1601.02687

