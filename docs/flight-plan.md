Flight Plan
===========
Spaceship commands are placed in a separate script file (called a "flight plan") 
and a reference is put in the scenario file. Spaceship scripts are sequential 
lists of actions. Each action has a time stamp. The action will not happen 
before that time. Actions happen sequentially as they appear in the file. 
It is illegal to have out of order time stamps in a flight plan. 

## Header

The flight plan starts with a header describing the spaceship
```
ship-name: Durga
max-acceleration: 10          # m/s^2 (a)
max-fuel: 100                 # arbitrary units (u)
fuel-consumption-rate: 0.1    # u / s / a 
```

And initial state
```
state: Landed                 # One of Landed, Flying, Crashed
pos: 150e6 0 0                # x, y, z in m relative to Solar System bary-center
att: 1 0 0                    # attitude
vel: 10 0 0                   # initial velocity
gps: earth 38.728521, -77.251199  # Lat, Long relative to body
                                  # only used if state is Landed 
```

Notes:
1. If a `#` (octhrope) is encountered anywhere the rest of the line is ignored
1. no variables can have spaces in them


## Actions
The header is followed by a list of actions in the following format

```
<timestamp>: <action> <argument1> <argument2> ...
```

1. Each field is separated by one or more white spaces
2. If a `#` (octhrope) is encountered anywhere the rest of the line is ignored
3. The timestamp is the [julian date](http://aa.usno.navy.mil/data/docs/JulianDate.php)


```
Action:        cancel
Arguments:     NONE 
Description:   If there is a currently ongoing blocking action, that action is
               stopped. Otherwise, it has no effect.
```


```
Action:        interaction-event
Arguments:     spaceship s, float r, string id
Description:   If the spaceship s comes within r m of us, on or after the action
               timestamp create an event labeled with id
Notes:         This causes this flight-plan to be *soft*-dependent on the 
               flight-plan of s: If flight-plan s changes from a timestamp 
               before this, this event check will be rerun.
```


```
Action:        interaction-remove-event
Arguments:     spaceship s, float r, string id
Description:   If the spaceship s comes within r m of us, on or after the action
               timestamp remove us from the simulation. Effective on or after 
               time stamp of action. This creates an event labeled with id
Notes:         This causes this flight-plan to be *soft*-dependent on the 
               flight-plan of s: If flight-plan s changes from a timestamp 
               before this, this event check will be rerun and the simulation
               will be clipped at this point if needed.   
```




```
Action:        launch
Arguments:     float h, float p
Description:   If the spaceship is "landed" on a body, then it takes off 
               vertically to a height h above the surface and then orients
               tangentially and keeps thrusting till it's periapsis is at
               least p. The reference is the body the spaceship was landed on.
               Has no effect if the spaceship is not landed.
Blocking:      This action blocks
Termination:   This action terminates (stops blocking) when any of the following occurs
               1. The periapsis is achieved
               2. Fuel runs out
               3. The spaceship is land-bound (landed, crashed, waiting for take off etc.)
               4. A cancel command is encountered
```

```
Action:        engine
Arguments:     float l 
Description:   Set engine level to l  [0.0, 1.0]
```

```
Action:        attitude
Arguments:     float x, float y, float z 
Description:   Set attitude absolute direction to (x, y, z)
```

```
Action:        rattitude
Arguments:     body b, float t, float p 
Description:   Set attitude relative to body b, at angle t degrees in 
               orbital plane, p degrees out of orbital plane
```

```
Action:        park
Arguments:     body b, float p, float a 
Description:   Park in orbit around body b with periapsis p and apoapsis a
Blocking:      This action blocks
Termination:   This action terminates (stops blocking) when any of the following occurs
               1. The periapsis and apopasis are achieved
               2. Fuel runs out
               3. The spaceship is land-bound (landed, crashed, waiting for take off etc.)
               4. A cancel command is encountered
Notes:         This parks the spacecraft in the plane it currently is in.
                
```

```
Action:        change-plane
Arguments:     TBD 
Description:   Cange plane of orbit
Blocking:      This action blocks
Termination:   TBD
```

Notes:

1. Blocking commands (like park) only release control if their termination condition
   is met or a `cancel` command executes
