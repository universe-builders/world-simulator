# Responsibility
* Responsible for allowing the discovery of a servicer of distribute-zone-simulation appropriate for a provided LayerType.

## Input
{
    layerType: LayerType  # The layer type 
}

## Output
{
    servicerIP: IP  # IP of the appropriate distribute-zone-simulation servicer
}

## Rationale
* There are N servicers for distribute-zone-simulation.
* Each servicer handles a set of LayerType's.
    eg one servicer might handle: [[Position, RigidBody], [Damage]]
    and another servicer might handle: [[Healing], [Spawning]]

## Uses
* A simulate-zone servicer needs to determine which servicer of distribute-zone-simulation to register with in order to have workload be distributed to it.

## Responsible Pod
* zone-manager

## Kubernetes Strategy
1. 2 pod instances are created using a deployment
2. active and passive are decided by attempting to grab lock 
3. passive connects to active and synchronizes state
4. passive marks itself for discoverability by adding a label that is selected via a service
    * service: discover-zone-simulation-workload-distributor
    * N passive pods will provide this service using the data synchronized from the active pod
