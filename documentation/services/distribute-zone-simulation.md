# Responsibility
* distribute the simulation of zones.

## Rationale
Simulation of components is distributed amongst N simulators that simulate a specific set of components. To provide further scale, the world is dynamically subdivided and distributed amongst many simulators that simulate the same set of components.

eg a physics simulator that simulates [Position, RigidBody] components may not have the required resources to simulate the entire world for all those components. As such the world is subdivided into M areas and responsibility for each area is delegated to a physics simulator.

## Responsible Pod
zone-manager, specifically only 1 pod which is in active-mode.

## Kubernetes Strategy
1. 2 pod instances are created using a deployment
2. active and passive are decided by attempting to grab lock 
3. passive connects to active and continously syncs state for fault tolerance
4. simulator discovers active and connects, providing a unique-ID to it
    * the active will not directly provide the responsibility to the simulator, instead that is handled by the service: view-component-set-simulators-area-of-responsibility
5. simulator discovers passive and connects, providing a unique-ID to it
    * this is done so that if passive becomes active it can remain as consistent to active -- if passive becomes active and any simulators disconnect then those are removed from their responsibilities
6. each area of responsibility should have a simulator in active and passive, if an area exists with no passive then the newly connected simulator is delegated the responsibility of being the passive, otherwise responsibility-provider figures out how to distribute responsibility to new simulator and provides that to the simulator (potentially removing responsibility from other simulators), which is active until the simulator loses connection to the responsibility-provider
