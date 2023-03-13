# Services
* component-simulator
    * Responsibilities
        * Simulates the components for an area within the world
        * Has authority over a set of component types, which is has sole write access to 
        * Maps entity ID to components, so it is able to delete components when an entity is deleted
    * Fault Tolerance
        * Future: have a cache
    * Elasticity: 
        * N area-simulator types which are orchestrated individually
        * N area-simulator's per world of the same type
        * CPU heavy
    * Connections:
        * Outgoing connection to component-authority-issuer for maintaining authority, if it disconnects it loses authority. If it ever tries to connect and is not needed then it will kill itself.
        * Incoming connection for retrieving component state filtered by an area (world-editor, component-view-provider)
        * Outgoing connection to component-view-provider for witnessing components on boundaries
        * Outgoing/Incoming when migrating to component-simulator
    * K8s Objects
    * 

* Future: position cache
    * lots of services need the position to determine routing
    eg entity-event-router

* component-simulator-spawner
    * Responsibilities
        * Spawns instances of component-simulator as required
    * Connections
        * Incoming component-authority-issuer : spawning component-simulators as load requires
        * Incoming entity-event-router : spawning component-simulators as component-types encountered that cannot be serviced by existing

* component-authority-issuer
    * Responsibilities
        * Gives component-simulators authority over an area
        * Ensures the entire world's area has component-simulators for each component
    * Notes
        * Based on the usage of component-simulator it will shrink the area of authority
    * Input
        * Components that simulator can simulate, this creates a layer which cannot have overlaps with another layer. ie two seperate layers cannot share a component type.
    * Output
        * Area boundary to simulate those components. Each edge also has information of the authority on the other side of the edge, eg left edge authority is X service
    * Notes
        * Splits an area horizontally or vertically based on its shape, vertical rects get split horizontally and horizontal rects get split vertically. It determines the shape of each area by tracking the transforms of 
        entities.
    * Failure Tolerance
        * Best: Queries all component-simulators for existing area boundaries.
        * Worst: Defines new authority area's on startup.
        * Future: Cache into a DB.
    * Scaling: 
        * N per world, each handling a different set of component-types.
        eg world-zoner-1: [physics-2D] world-zoner-2: [AI, damage]
        this can be determined by ordering them deterministically and if 2 exist then 0th will do % 2 == 0 and 1st will do % 2 == 1
        * CPU heavy
    * Connections
        * Incoming persistent connection for authority state
            * This should be done in a way that is generic, ie no difference between component-view-provider and component-simulator
        * Outbound to component-view-provider
            * Gets the actual bounds of the world by looking at the transforms of all the entities, this should be done at a low frequency. The component-view-provider could then do more complex splitting schemes, such as finding groups and splitting around those.


* component-view-provider
    * Responsibilities
        * Provides a view into the data of components in an area
        * Gets the data from the authority of the component
    * Input
        * Area (can be updated, eg when a camera moves)
        * Component Filter
        * Entity Filter
    * Failure Tolerance
        * Stateless, re-scheduled
    * Scaling: 
        * N area-simulator-view-provider per area-simulator
        * Network heavy
    * Connections:
        * Outgoing connection to component-simulators for component state
        * Outgoing connection to component-authority-issuers for authority state



* entity-event-router
    * Responsibilities
        * Routes an event to do with an entity (or component therein) to the correct destination
        * Handles the complexity of shifting workload distribution eg component-simulators having different authority regions
    * Users
        * component-simulator -> component-simulator
            * will emit component events to other component-simulators, needs to be low-latency - fire-simulator sends damage to damage-simulator which should be highly
        * component-simulator -> entity-lifecycle-manager
            * will emit entity events, such as deletion and creation.
        * world-editor -> component-simulator
    * Connections
        * Outgoing connection to component-authority-issuers for authority state
        * Incoming connections for event routing
    * Scaling: 
        * N per world, stateless.
        * CPU heavy.
    * Fault Tolerance
        * May lose some state eg events in flight that may need to be resent
    * Notes
        * It could be a daemon and local pods could use it

* entity-lifecycle-manager
    * Responsibilities
        * Responds to entity events and ensures the correct components are created/deleted on all the appropriate component-simulators
        * Instantiates entites based on entity-snapshots, which are synced from GitHub continously

    * Scaling
        * Stateless
        * Manages a subset of entity ID's depending on number of entity-managers, query the K8s API to determine it's subset. Based on SS if id==1 and theres 3, then id % 2 == 0 it manages

* world-snapshotter
    * Responsibilities
        * Takes a snapshot of all the component-simulator state.
        * Pauses entity-life-cycle manager.
        * Pauses all component-simulators.
    * Notes
        * Is a job.

* world-snapshot-initializer
    * Responsibilities
        * Goes through world-snapshot (array of entity -> components) and creates each using an entity-lifecycle-manager.
        * Pauses all component-simulators until completed.
    * Notes
        * Is a job.

# Notes
* World creation happens by applying a manifest with all necessary services.
* Should use UDP for communications, with idempotecy, ACKing, and resending.
* component-simulator migration (simple):
    1) Authority tells A to migrate to B
    2) A stops simulating and sends latest snapshot to B
    3) A sends all events to B
    4) A tells authority migration is completed
    5) Authority expand's B's area to include A, removes A
    6) A sticks around for N seconds for in-flight events and forwards them to B
    7) A kills itself <HOW? it is a pod...>

# component-simulator splitting strategy
* If a simulator is under load then it's authority will be split into two.
* component-authority-issuer will 


# Requirements
* Components should be cloud-native, ie:
    * fault tolerant
    * elastic and auto-scalable
    * rolling-updatable
* Should be able to run in single area mode, including on web client only, ideally without having to specify a mode -- it just shouldn't orchestrate certain components.
    * component-authority-issuer is the only one that needs to be changed, could potentially have a query of the number of nodes that a world can be orchestrated on.

# To Do

? Where could fault tolerance issues of event-router be a problem (eg deleting components)

-> Ensure each service has all fields (Fault-Tolerance, ..)

-> UDP in K8s
-> Create a DFD using mermaid
-> Use snapshots with IDs for migrating, versioning, and 



* I think this architecture does not fit well with K8s, specifically:
    * Pods can not kill themselves
    * Perhaps when authority gets a component-simulator connection it determines the authority, when a connection dies it handes off authority. It deals with the ephemeral nature of pods

    * Underlying issue: component-simulators are not architected to be ephemeral. Perhaps this is a big reason for having a spatial-db style system, which allows component-simulators to be more ephemeral.

    * Maybe theres a zoning algo that works well with SS, they slide up over time or something to allow FIFO.





StatefulSets destroy in order which leads to complications of scaling down

Option: Use deployment and have them connect to component-authority-issuer which will issue their authority.
    - complexities around component-authority-issuer splitting.
    + when a component-simulator disconnects from component-authority-issuer it loses authority 


