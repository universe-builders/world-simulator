# Services
* discover-zone-simulation-distributor 
    * Allows discovery of an appropriate servicer of distribute-zone-simulation
* distribute-zone-simulation
    * Distributes simulate-zone service amongst zones  

---

# Managed Resourcecs
* zones
    * All zones for N layers.

---

# Channels
* active
    * N passive
        * Each passive connects to active to get a syncronized view of the state for fault tolerance and for offloading the viewing workload.
        * If no passive it provides service: view-component-set-simulators-area-of-responsibility
    * N component-set-simulators
        * Each component-set-simulator gets an area of responsibility.
* passive
    * N viewers
        * Each viewer gets a syncronized view of the state.
    * N component-set-simulators
        * This is done so that if passive becomes active it remains consistent to active.

---

# Replica Oriented
* One active
    * services: distribute-zone-simulation
* N horizontally auto scalable passives
    * services: discover-zone-simulation-workload-distributor

---

# Fault Tolerant
* Passives will synchronize with active and undertake active services upon being promoted to active

---

# Disaster Recovery

---

# Graceful migration on termination
1. active stops making decisions around workload-distribution
2. active sends latest snapshot to all passives
3. active awaits ACKs from passives, or some timeout
4. active releases lock, a passive obtains the lock and becomes the active
    4.1 simulate-zone servicers lose connections to the active
    4.2 simulate-zone serviers continously query for new active via discover-zone-simulation-workload-distributor service
    4.3 simulate-zone serviers continue simulating optimistically assuming their existing responsibilities will be upheld by the new active
    4.4 when simulate-zone servicers receive active information, ideally they will have an existing connection to the passive-turned-active and their zone responsibility will be upheld. If no connection exists, then they will connect and likely receive their responsibility from the service.
5. terminates

---

# Ungraceful termination

1. active is terminated and thus releases lock, a passive obtains the lock.
2. new active should ideally have a connection to each component-simulator thus meaning all existing responsibilities can be adhered to.
    * worst case is that the new active has no new connections and needs to delegate responsibilities from scratch.
3. new replica is spawned and becomes passive as cannot obtain the lock.

---

# Simulator disconnects from distribute-zone-simulation
* simulator disconnects from active
    1. if simulator has a passive, active delegates the responsibility to the passive
    2. if not, active delegates the responsibility to a connected simulator
        * likely merges the responsibility with a nearby simulator


---

# Horizontally auto scalable
* Not entirely horizontally auto scalable due to distribute-zone-simulation being a singleton service. If this becomes a problem in large worlds then a pod which deletegates responsibilty of this service based on LayerType will be orchestrated. This is future proofed using discover-zone-simulation-workload-distributor service which allows that new pod to seamlessly be integrated without changes to any other pod.

---

# Rolling updatable

---

---

# To do
* Allows regions to be defined which are not increased in size and responsibility is provided to component-simulator's which have no other responsibilities.
    * Used in cases of world bosses, where a special area is reserved where the simulators are not given responsibility outside that area to ensure low latency and no thrashing.

---

# Service Checklist
* ✅ Responsibilities defined
* ✅ Replica oriented
* ✅ Fault Tolerant
* ⭕ Disaster Recovery
* ✅ Graceful migration on termination
* ⚠️ Horizontally auto scalable
* ⭕ Rolling updatable
* ✅ Channels defined

---