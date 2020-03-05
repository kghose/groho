# Developer documentation

## Use case diagram

![](uml/png/usecases.png)


## Sequence diagram

![](uml/png/sequence.png)

While the sequence diagram shows the scenario file being repeatedly loaded and
the simulation repeatedly restarted, internally we look for opportunities to
cache and reuse computation. So depending on if and how the scenario file
changes the `restart` command may mean the simulation just carries on, or it
restarts from a previous checkpoint in the simulation. Similarly for the
plotting, if the data is unchanged, or if the plotting file is unchanged, the
plot is not redone.