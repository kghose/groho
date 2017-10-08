Groho (গ্রহ)
=====
Groho is a simulator for inter-planetary travel and warfare.

The simulator aims to give the user intuitions of how inter-planetary travel, 
commerce and conflict would play out in the near future within the solar system. 
There is no faster than light travel, though there are no limits to how
powerful spaceship propulsion can be. 

Simulations are non-interactive: the user sets up a scenario and spaceship
control scripts and the simulation evolves according to natural law and
script actions. The program makes it easy to setup, run, save, modify, inspect 
and compose (combine together) simulations. 

*Groho (গ্রহ) is the Bengali word for planet. Grohomondol (গ্রহমণ্ডল) is the word for 
planetary system but is more of a mouthful.*


Typical scenarios
-----------------
## Testing out a simple Earth-to-Moon transfer
- The spaceship's initial position is specified as lat/long coordinates
- The scneario file refers to a spaceship model, which is described in a separate 
  spaceship model file
- A spaceship script is written
  - The script indicates time stamps and actions
  - Actions are one of change engine thrust or change attitude
  - The actions will be honored as long as the spaceship is
    capable of them

## Earth-to-Mars transfer
- This is exactly the same as for Earth-to-moon

## Compose simulations
- We can now take the simulation result files from both simulations and
  overlay them to visualize them together
- We can also paste the scenario files togther (concatenate them) and 
  rerun them for the same effect

## Simulating communications
- We can send a 'signal' from one spaceship in a certain direction
  - The signal is modeled in a signal model file
  - The transmission is timestamped and when any other spaceship
  - 
- We take the composed simulation from above


## Simulating 





Notes
=====
1. 
