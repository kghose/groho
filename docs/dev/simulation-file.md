Serializing simulations (v0.0)
==============================
(Serializing means saving and loading simulations from disk)

The interleaved data format, where we just drop heterogeneous data structures
as we generate them, is convenient from a writing point of view, but makes
reading awkward, especially for javascript. What seems to be more convenient 
is to save data in blocks of homogeneous types.

We pick a block size, guided either by simulation time or based on how much
data we've generated so far. When we have a complete block we dump each class of 
data as a contiguous section.

For each block we save, in order, object positions, events and full object 
state.

```
# HEADER

# DATA BLOCK

    * pointer to previous block
    * pointer to next block
    * t_start
    * t_end

    * Nbp - number of bodies for position data

        # POSITION BLOCK

        * spkid  - id
        * N - number of points
        * t0, t1, .... t(N-1)
        * x0, y0, z0, x1, y1, z1, .... x(N-1), y(N-1), z(N-1)

        # POSITION BLOCK REPEATS
    
    * Ne - number of events
    * t0, t1, ...., t(Ne-1)          - event times
    * id0, id1, ...., id(Ne-1)       - object A ids
    * id0, id1, ...., id(Ne-1)       - object B ids
    * code0, code1, ...., code(Ne-1) - event code
    
    * Nos - number of bodies for full object state

        # OBJECT BLOCK

        * spkid  - id
        * N - number of points
        * t0, t1, .... t(N-1)
        * x0, y0, z0, x1, y1, z1, .... x(N-1), y(N-1), z(N-1)
        * vx0, vy0, vz0, vx1, vy1, vz1, .... vx(N-1), vy(N-1), vz(N-1)
        * tx0, ty0, tz0, tx1, ty1, tz1, .... tx(N-1), ty(N-1), tz(N-1)        
        * f0, f1, .... f(N-1)

        # OBJECT BLOCK REPEATS

# DATA BLOCK REPEATS
```


Positions
---------
These the only data saved for orrery bodies. This data can also be saved for
simulation bodies. 

spkid
N
t        (double) * N    N * 8
pos      (double, double, double) * N  N * 3 * 8












we pick a block size that corresponds to
a certain maximum number of points and save the data as follows


header

for every object ----
spkid       int 4
N        uint64 8
t        (double) * N    N * 8
pos      (double, double, double) * N  N * 3 * 8
vel      (double, double, double) * N  N * 3 * 8
thrust   (double, double, double) * N  N * 3 * 8
fuel     






t    double   8  
id   int      4
x    float    4
y    float    4
z    float    4
vx   float    4
vy   float    4
vz   float    4
tx   float    4
ty   float    4
tz   float    4
f    float    4
e    uint32   4

12 * 4 + 8 = 56 bytes per data point












My first instinct is to write my own serialization format. This is understandable
and wrong. It's fun to think about, to marvel at how well C++ will do serialization
of POD (both reading and writing, including strings) but considering that I'm
thinking about using a javascript browser-based app for visualization, and also
may code up other visualizers and it's just this incidental thing that I need to
do on my way to more fun things, the most sensible path is to use an existing
and widely adopted serialization method. Such a method will have, preferably,
a header only C++ implementation and an efficient javascript implementation.

There are many options out there as a web-search will reveal.

Google's [Protocol buffers][g-pb] look very heavy weight as does [Avro][avro].
Both of these seem to require installing a library, compiling a schema to code 
which is then included in the source and so on. Avro also needs boost.
Google's [Flat buffers][g-fb] were also interesting. 

I rather liked [MessagePack][msg-pak] - it's byline hit the spot
> MessagePack is an efficient binary serialization format. It lets you exchange 
> data among multiple languages like JSON. But it's faster and smaller. 

And these two features really grabbed my attention:
- it's header only - you just have to include their series of headers into 
your source.
- You can serialize/deserializes user-defined classes using MSGPACK_DEFINE macro

[g-pb]: https://developers.google.com/protocol-buffers/?hl=en
[msg-pak]: https://msgpack.org/
[avro]: https://avro.apache.org/
[g-fb]: https://google.github.io/flatbuffers/


Notes:
=====

The problem I faced was that both javascript msgpack implementations would not 
decode more than the first object in the stream


Organizing the data
-------------------


Data in the file is stored as header followed by a series of packets. 

The header contains information about the simulation and the bodies that will
be found in it.

There are three kinds of packets:

Position packets 
    - created to allow smooth interpolation of body and spacecraft positions

State packets 
    - created when there are changes in a spacecraft state, like changes 
      in thrust vector or significant changes in fuel level

Event packets
    - created when an event involving one or more objects occurs

By the nature of how they are created, packets are sorted by time and random
access into this file is possible using a binary search strategy.








Each packet is an object 
position or an event padded to the same length, such that all packets are of the
same size.

// position
struct Packet {
    char    p;        //  type of packet
    int64_t t;        //  time
    int32_t spk_id;   //  ID
    double x;         //  pos
    double y;         // 
    double z;         // 
};                    

If this is a spacecraft we additionally have the following information

// space ship state
struct Packet {
    double f;         // fuel level
    double x;         // engine thrust vector
    double y;         // 
    double z;         //
};






The header lists all the orrery bodies we will encounter along with their 
relevant properties.

```
struct Body {
    int32_t     spk_id;
     name;
    double      GM;
    double      r;
};
```

This is followed by positions 



struct Pos {
    int32_t     spk_id;
    double t;
    double x;
    double y;
    double z;    
}

struct 



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

