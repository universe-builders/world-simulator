# To Do

* Implement active-passive election in C
    * Rename Process_Control, TCP's, with new naming convention in fns.
    * Proper dynamic array/list setup for client sockets (how is it used?).
    * Heartbeat.
    * Disconnect client in TCP_Server properly (on fail write/read etc).
    * Integrate K8s.

* service: distribute-zone-simulation
    * Implement active-passive election in C
    * Add sidecar for active-passive election
    * Grab active lock for service distribute-zone-simulation
    * Accept TCP connections
    * Test with a local client
    * Implement dev-env for mock-zone-simulator
    * Connect mock-zone-simulator via TCP and service distribute-zone-simulation
    * Message between mock-zone-simulator and zone-manager with zones
