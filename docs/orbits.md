Orbit simulations
=================

I'm using NASA's open source and gorily documented [GMAT][gmat] software as the gold standard for solar system simulation. A quick peruse of their [mathematical specifications document][gmat-spec], found in [their contributors page][gmat-contrib] shows how complicated a proper simulation is, and requires several years worth of background. The first chapter, for example, is solely on the topic of time and how to measure it.


[gmat]: http://gmatcentral.org/
[gmat-spec]: http://sourceforge.net/p/gmat/code/HEAD/tree/trunk/doc/SystemDocs/MathematicalSpecification/GMATMathSpec.pdf?format=raw
[gmat-contrib]: http://gmatcentral.org/display/GW/For+Contributors

My attempt here will be to start with rather simple models of solar system dynamics but design the program so that more complex models can be easily swapped in as my understanding improves.


Interpolated ephemerides
------------------------
- Retrieve ephemerides from NASA-JPL's HORIZON system
    - Ephemeris Type : VECTORS
    - Target Body : ??
    - Coordinate Origin : Solar System Barycenter (SSB) [500@0]
    - Time Span : 	Start=2100-01-01, Stop=2199-12-31, Step=??
    - Table Settings : quantities code=1; output units=KM-S; CSV format=YES
    - Display/Output : plain text
    - Try out Python package 'callhorizons' ([blog post][ch-post], [docs][ch-docs])
        - Not convenient for x, y, z format I want
        - can extract out just the bits I need easily
- Use interpolation between points to compute body position at given time
- The step size for the ephemerides depends on the orbital period and how irregular the orbit is. The aim is to have a maximum error of 1km between the interpolated and actual orbits.

### Data file format

The data file should store the body's name, mass, radius, axis tilt, angular speed and the ephemeris data. This allows a simulation to keep track of both the center of the object and the location and direction of any surface features (computed by accounting for rotational angle and axis tilt). Names should be unique as they are used to identify the objects and determine anchors for fixed objects.

[ch-post]: https://michaelmommert.wordpress.com/2016/01/08/a-python-module-to-query-jpl-horizons/
[ch-docs]: http://mommermi.github.io/callhorizons/readme.html


Keplerian approximation
-----------------------
The plan for this is to use the [data][nasa-kepler-data] and [formulae][nasa-kepler-formulae] presented on NASA's [Keplerian Elements for Approximate Positions of the Major Planets][nasa-kepler] page.

[nasa-kepler-data]: http://ssd.jpl.nasa.gov/txt/p_elem_t2.txt
[nasa-kepler-formulae]: http://ssd.jpl.nasa.gov/txt/aprx_pos_planets.pdf
[nasa-kepler]: http://ssd.jpl.nasa.gov/?planet_pos

This document only specifies planetary orbits and not those of satellites of planets. 