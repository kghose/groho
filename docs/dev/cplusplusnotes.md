Notes on C++
============


Threading
---------
When passing a class method as the function to run in a thread use the following
syntax (used in `main.cpp`, for example):

```

  class Simulation
  {
    ...
    void loop() {;}
  }


  sim::Simulation simulation( scenario_file );

  ...

  std::thread simulation_thread( 
    &sim::Simulation::loop,
    std::ref( simulation )
  );
```
Thread wants to make a copy of the object it's being passed, so if you would
rather that the live version of the object be used, you should pass a reference
using std::ref