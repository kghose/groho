These structs and methods will read the scenario and flight plan files and store
them in such a manner that

1. The simulator can decide if we need to re-simulate and for what and from when
2. The simulator can parse this to create a new simulation. 

These structures do not, however, store the actual Orrery data, or the actual
flight plan objects. A further level of processing is required to instantiate
these objects. 

The Orrery data needs to be loaded from disk if needed, while the flight plans
can be further parsed from this structure alone.

This has been done to prevent too much coupling between data structures and
algorithms.