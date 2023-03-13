# Services
* component-simulator
    * Responsibilities
        * Simulates the components for an area within the world
        * Has authority over a set of component types, which is has sole write access to 
        * Maps entity ID to components, so it is able to delete components when an entity is deleted
    * Fault Tolerance
        * Backs up to world-view at a low frequency
    * K8s
        * Workload Resource: Deployment
    * Elasticity
        * Horizontal Pod Autoscaling
        * One plane per X-component-simulator
        * N different component-simulators for seperate logic (physics, damage)
    * Bottleneck
        * CPU

* Future: world-view
    * Responsibilities
        * Stores the state of all the components within the world for purposes of fault tolerance.
    * K8s
        * 
    * Fault Tolerance
        * Have an active and a passive




# Flows
* component-simulator spawned
    1. Connects to component-authority-issuer
    2. component-authority-issuer issues authority for an area within that component-simulator plane
* component-simulator gracefully deleted (controlled migration)
    A: component-simulator being gracefully deleted
    B: component-simulator gaining authority area

    1.  A requests migration from component-authority-issuer
    2.  component-authority-issuer messages B to migrate data from A
    2.1 A stops simulating and processing events
    2.2 A sends full snapshot to component-simulator B
    2.3 A continues forwarding all events to component-simulator B until death
    3.  B indicates to component-authority-issuer migration is completed
* component-simulator ungracefully terminated (backup)
    1. component-authority-issuer scales authority of other component-simulators in the plane
    2. component-simulator queries world-view for the area it gained authority over
* new authority is added
* authority is terminated



Option: Upon authority expanding component-authority-issuer provides migrator

Option: world-view is the mechanism used to migrate
    * gracefully deleting component-simulator will backup data to world-view
    * component-simulator disconnects from component-authority-issuer and dies gracefully
    * component-authority-issuer issues authority over area to an existing component-simulator
    * component-simulator queries world-view for area

Option: Active Passive
    - Chaining K8s requests, race conditions with K8s deployment

(own controller would be ideal)
* For each active component-simulator have a passive, on component-simulator death then move passive to active deployment via labels.

# Todo
* What services should be colocated and how should this be achieved? 

* Should I mark any services as critical to help avoid eviction?

? Would it be useful to have a general component-simulator that pulls down a binary to become X-component-simulator as opposed to defining pods with them

* Could have active-passive for each defined area, passive awaits lock and syncs with active. The passive could do the filtering logic and networking, or they could both be available for networking and the passive relays to active.

? Is Horizontal scaling an appropriate option (especially with active-passive) or should there be a spawner

* Shouldnt use SS for identical pods -- don't rely on existing pattern. Instead should do something akin to having locks for each resouce. This creates a more appropriate load balancing scheme, opposed to use hashes to determine which on SS to interface with.

* Seperate into different problems
    * Authority
    * Authority issuing
    * ...

* Can use K8s labels to annotate Pods and then use those to determine roles (active, passive, ..)

* Could use a controller/operator for authority-issuing

# Issues with v1
* Architecture should be as close to kubernetes native as possible, or if there are too many compromises then I should consider rolling my own controller.
* There should be more of an approach to orchestrate resources as they are available, and when removed then delegate workload elsewhere.
* Containers should not control their lifecycle.

