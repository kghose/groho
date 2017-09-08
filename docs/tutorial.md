Tutorial (v0.0)
===============

The simulator requires an orrery file which stores the orbital elements for the planets suitable for computing approximate positions of the planets. I use the numbers from E M Standish's short paper. This is found in data/keplerian-elements-standish.txt




A new simulation is started by executing

```
groho init parameters.txt -o sim-file.gro
```

After the initialization, the parameter file is not needed - it is stored as part of the simulation file. The original parameters can be extracted from the simulation file by executing:

```
groho print-parameters sim-file.gro > copy-of-parameters.txt
```

 
