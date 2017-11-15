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
  - FLTK 

macOS: `brew install fltk`  
Linux: `sudo apt-get install fltk`  

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


Thanks
======
Included in the code are the following fine pieces of software

1. [loguru](https://github.com/emilk/loguru) from Emil Ernerfeldt