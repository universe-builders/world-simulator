# Rationale

# Responsibilities
* simulate-component-set-in-area (active)
* view-component-set-in-area (passive)

# Elasticity 

# Channels

* shared
* active
    * N passive
        * Each passive connects to active to get a syncronized view of the state for fault tolerance and offloading the viewing workload.
* passive
    * N viewers
        * Each viewer gets a syncronized view of the state.
    * N component-set-simulators
        * This is done so that if passive becomes active it remains consistent to active.

