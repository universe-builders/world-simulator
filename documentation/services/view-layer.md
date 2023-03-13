# Responsibility
Provide a view into the responsibility areas of all component-set-simulators.

## Rationale
To get a view into an area in the world services need to resolve which component-set-simulators are responsible for the components and area the viewer is interested in, ie a player is interested in a view of the world in the immediate area around them.

This service provides all the component-set-layers which are subdivided into N area's each of which a component-set-simulator is responsible for.

## Responsible Pod
The component-set-simulator-responsibility-provider pod provides this service, specifically only pods in passive-mode or in active-mode with no passive-mode connections.

## Kubernetes Strategy
1. 2 pods are created using a deployment
2. active and passive are decided by attempting to grab lock 
3. passive connects to active and synchronizes state
    * State includes component-set-layers, subdivided areas, and component-set-simulators responsible for those areas.
4. passive marks itself for discoverability (service)
    * service: view-component-set-simulators-area-of-responsibility
    * N passive pods will provide this service
5. when incoming connection occurs, passive sends through the latestest snapshot and all updates
    * subscribers can then use this information to connect to the correct component-simulator for a view into their component-set.
6. when passive becomes active and has a passive connection, it drops this responsibility

## Web Strategy