# Scenario file manual

The scenario file lists out the initial conditions for the simulation, the orrery
model and the flight plans for any spacecraft in the simulation.

The easiest way to learn about scenario files is to look at the annotated
[example](examples/basic-scenario.txt) we just ran.

To get a list of spacecraft programs and how to use them do
```
build/groho programs
```

## Orrery model
You pass a list of kernel files to the simulator. Optionally, you can indicate a
subset of NAIF codes that the simulator should load from the kernel file.

```
spk de432s.bsp ; load everything from this file

pick 809 899
spk nep086.bsp ; only load 809 and 899 from this file
```

A SPK file contains barycenters as well as physical bodies. Often ephemeris are
stored relative a barycenter. For example:
```
301 -> 3 -> 0
299 -> 2 -> 0
809 -> 8 -> 0
899 -> 8 -> 0
```

When loading objects the simulator follows the following rules

1. If a barycenter and the main body of the barycenter are both loaded, the
   barycenter is used for coordinate transforms but is not used for gravity
   computations: the main body is used. The trajectory of the barycenter is
   also, in this case, not saved.
1. If a body encountered more than once, the ephemeris is loaded from the first
   file it is encountered in.

In the given example, the barycenter 8 is not used for gravity computations
since 899 is loaded. 


## Flight plans

Flight plans start with a line indicating the name of the spacecraft

```
plan Durga
```

This is followed by a list of **events**. Each event specifies a start time,
duration and, a spacecraft **program** and how it should run. For example the
line: 

```
2050.01.01:0.5 3600 orbit 301 200x200
```

will turn on a program that thrusts the spaceship till it achieves a 200x200 km
orbit around the moon or till the duration is up, whichever is earlier. (A
duration is required because this makes restarts easier. It allows the simulator
to more easily decide the time point upto which computations can be reused)

Each program has access to the state of the solarsystem (modeling a perfect IMU
and perfect knowledge of the solar system) and produces only one output: a
thrust vector for the spaceship.

Multiple programs can not run at the same time. If a program's event time occurs
before the previous program has terminated, it will be put into a
queue, so that they will run as soon as the earlier program has finished.

To describe another spacecraft's flight plan, simply use another `plan`
statement. All events coming after this, will be associated with this new
spacecraft. 

## The `insert` directive
`insert` followed by a file path inserts the text of that file into the original
file at that point. This can be done recursively. In this manner, multiple files
can be combined to form the complete scenario file. For example 

```
start 2050.01.01:0.5
end 2055.01.01:0.5

spk de432s.bsp
pick 809 899
spk nep086.bsp 

insert flightplan1.txt
insert flightplan2.txt
```
is a neat way of splitting out the flightplans of the two spacecraft in the
simulation into two additional files. This can make the writing of the
simulation more manageable.

# Plot description file manual

```
fig:
  figsize: [6, 7]

panels:
  main:
    subplot: 211
  moon:
    subplot: 212
    ref: 399
    targets: [-1000, 301]
    dt: 60
```

Description files are YAML files. They contain two top level sections: `fig` and
`panels`. The `fig` section indicates overall figure properties.

The `panels` section is a dictionary of suplots that we want drawn. The
`subplot` command indicates where the plot should be placed on an imaginary
grid. So `211` means, "on an imaginary grid 2 rows x 1 column, make this the
first plot"

If nothing else is supplied this plots all the data in the simulation referenced
to the solar system barycenter.

If `ref` is passed the plot is translated to have the body indicated by `ref` in
the center. If `targets` is passed (this has to be a list), only that list of
bodies are plotted. If `dt` is passed, the trajectories are spline interpolated
with an interval of `dt` seconds.
