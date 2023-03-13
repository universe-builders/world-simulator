# Responsibility
Simulate a component-set within an specified area, specified by service distribute-workload-amongst-component-set-simulators.

## Responsible Pod
component-set-simulator

## Kubernetes Strategy
<MERMAID>

1. simulator is deployed with a minimum replica of 1 and HPA scales up as required.
2. simulator resolves workload-distributor's (active, and N passives) by accessing service query-for-component-set-simulator-workload-distributor
3. simulator connects to workload-distributor (active) and is assigned the role of active or passive
    * active: gets responsibilities and simulates component-set for responsible area
    * passive: connects to active and synchronizes data, provides view
4. simulator connects to N workload-distributors (passive)
    * used for cases when workload-distributor-active is terminated, passive-turned-active will have connection and will uphold existing responsibilities

## Events
* workload-distributor informs passive that it as responsibilities and thus becomes active
    1. if active has a passive, it drops all passive responsibilities (viewing)
    2. active begins simulating

* active is gracefully terminated
    1. active stops simulating
    2. if passives, active syncs all passives with latest snapshot
        2.1. awaits ACKing from passives, or some timeout
    3. if no passives, active requests a migration from workload-distributor
        3.1 workload-distributor offloads responsibilities to another simulator, informs active
            * should set active absorbing responsibility to high priority of not being terminated as it would be complex to handle the case where it is having data migrated to it and then it is gracefully terminated too.
        3.2 if no other simulators, log error
        3.3 active connects to migrates all data to simulator, awaits ACK
        3.4 active disconnects from workload-distributor and it then assigns the responsibility to the new simulator
    4. disconnects from workload-distributor, which then assigns passive to active if one available or assigns the migrator to active
    5. terminates

* active is ungracefully terminated
    1. if passives, workload-distributor assigns responsibility to them
    2. if no passives, throw error and destroy all entities in the area?

* passive is gracefully terminated
    1. passive disconnects from workload-distributor
        1.1 active is made aware that it lost a passive, even if passive is still connected via channel it stops syncing it -- active only syncs passives that are connected and are specified by workload-distributor
    2. if active has no passives, it handles all passive responsibilities (viewing)
    3. passive disconnects from active