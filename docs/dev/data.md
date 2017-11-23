Data considerations
===================

Storing data at each timestep
-----------------------------
For display purposes each object requires 3 floats per display point (x, y, z) 
which is 12 bytes. For a simulation with 100 objects this is 1200 bytes per 
time step. If we save this data once every second then for a 1 year simulation 
we have 365 * 24 * 60 * 60 * 1200 = 37843200000 bytes = 35.24 GB, or 
0.3524 GB/object uncompressed. We could do this for a simulation of say 20 objects
-> 7 GB but, unless we do something clever with orrery objects most of this will be 
taken up by the solar system!

Sparse, adaptive, timesteps
---------------------------
For display purposes if we do linear interpolation between points we will be able
to get by with an adaptive scheme where we save timestep data only if the linear
interpolation between that and the previous point has a maximum (or mean) error
above a certain threshold. 

If we make the threshold adaptive such that when an object is far away from
other objects we allow sparse points and when other objects are close by we allow
denser points we could potentially be even more efficient

**Remember:** The simulation is accurate (to the resolution of the timestep
we use) and the saved display data points are accurate, it's just the interpolation
between the display points that has an error.

### Savings

The data now has an additional float value for the timestep, so each point now
costs us 16 bytes. We need to save less than 75% of the original number of 
timesteps for this to be an advantage.

As we can see from a [quick experiment](adaptive-display-points.ipynb) the savings
can be tremendous if our simulation is finegrained. In the example shown with
just 1% of the original points we get a very smooth display.

What are we saving data to?
---------------------------
Do we want to be general and save the simulation data to a serialization format
that can then be used by the built in display as well as any external program
that we choose? Or, do we say YAGNI and stick to an internal format specific for
OpenGL? 

In my head, at some point, I would like to print out simulations as vector graphics
but that seems to be pretty involved. In all likelihood that should be a separate
feature that reruns the simulation and re-renders it to a SVG backend.


Checkpoints
-----------
Since the simulations are completely determined by the scenario and flight plans we can avoid serializing the simulation if we are happy with recomputing it as needed, with the caveat that a long simulation may keep us waiting. 

Almost the entirety of this is however governed by display needs. In the end we want to visually inspect the flight paths. For this purpose it will be sufficient to store checkpoints at intervals where a linear or other interpolation will suffice for display purposes.

The simplest scheme, which we will start with, is to save checkpoints when the maximum deviation between the simulation and a linear interpolation exceeds some threshold, such as 10m, which can be different for each body.

This means that we need to store one extra double (the julian date) so we have 32 bytes per checkpoint.

_Note that this discussion is about serialization (saving data) and display. The simulation time step is independent of this discussion, and in this first iteration there is just a constant simulation time step._


Scaling data for display
========================
If we wish for 1m resolution at solar system distances (40 AU for pluto) we need
to use doubles assuming 1 unit represents 1m. 40 AU = 5.98391483e12 m and doubles
give us integer resolution to [9007.199254740992e12 m][double-prec]. While a 
float would give us integer resolution to [16.777216e6 m][single-prec] only.

[double-prec]: https://en.wikipedia.org/wiki/Double-precision_floating-point_format#IEEE_754_double-precision_binary_floating-point_format:_binary64

[single-prec]: https://en.wikipedia.org/wiki/Single-precision_floating-point_format#Precision_limits_on_integer_values

This is fine for the simulation, but there are other considerations for the
display. OpenGL commonly accepts data as floats and the large numbers we handle
here would cause problems in terms of the matrix computations. For a fun 
discussion related to all this see [here][large-environment]. The solutions 
proposed there are complex and are needed when plotting textures and handling
movements and interactions at different scales.

For our purposes I decided that it would be easiest to scale the data before
sending it to OpenGL. It works well so far and I'll revisit it if needed.

[large-environment]: https://www.gamedev.net/forums/topic/557264-confused-very-large-environments/





