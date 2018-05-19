Groho ( গ্রহ )
=====
Groho is a simulator for inter-planetary travel and warfare.

The simulator aims to give the user intuitions of how inter-planetary travel, 
commerce and conflict would play out in the near future within the solar system. 
There is no faster than light travel, though there are no limits to 
how powerful spaceship propulsion can be. 

Spacecraft actions are defined using scripts called flight plans. The user
defines the solar system model, initial conditions (such as number and disposition
of spaceships) and the simulation evolves according to natural law and
flight plan actions. The program makes it easy to setup, run, save, modify, inspect 
and compose (combine together) simulations. 

*Groho (গ্রহ) is the Bengali word for planet. Grohomondol (গ্রহমণ্ডল) is the word for 
planetary system but is more of a mouthful.*

Compilation
===========
Dependencies: 
  - Magnum/Corrade 

macOS: `brew install mosra/magnum/magnum` and `brew install mosra/magnum/corrade`

Compile:
```
git clone git@github.com:kghose/groho.git
cd groho && \
mkdir build && \
cd build && \
cmake -DCMAKE_BUILD_TYPE=Release .. && \
make
```


[Tutorial](docs/tutorial.md)
=========
The tutorial is the launch point for learning more

[Docs for Devs](docs/dev/Readme.md)
===============
Notes, mostly for me six months in the future, about code design and algorithms


Related software
================

NASA's GMAT
-----------

NASA's [GMAT] software ([github][GMAT-github]) is probably the be-all and end-all 
of spacecraft simulation. It would probably have sufficed for my needs 
but I like to program computers and learn math, 
so I stubbornly rolled my own to my own (much more limited) specifications.

[GMAT]: http://gmatcentral.org/display/GW/GMAT+Wiki+Home
[GMAT-github]: https://github.com/haisamido/GMAT

Bussard by Phil Hagelberg
-------------------------
[Bussard](https://technomancy.itch.io/bussard) is a cool hacking/spaceflight game
where you write complex programs to fly your spaceship to solve puzzles.

SolarSystemOrbiter
------------------
[SolarSystemOrbiter](https://github.com/madoee/SolarSystemOrbiter) -
"Plot the orbits of the planets in our Solar System and calculate the Hohmann Transfer Orbits to transfer your rocket ship from one planet to the other and back." 


Thanks
======
Included in the code are the following fine pieces of software

1. [loguru](https://github.com/emilk/loguru) from Emil Ernerfeldt
2. [catch(2)](https://github.com/catchorg/Catch2) from Phil Nash
3. [magnum/corrade](https://http://magnum.graphics/) from Mosra (Vladimír Von­druš)