# engine.world

# Engine

## To Do
* 🎯 World DB
    * Dev Env
    * Building (cntr)
    * Orchestrated
    * Entities
    * Components
* 🎯 C System
    * ✅ Dev Env container vscode
    * ✅ Really simple C system
    * Building
        * ✅ Build binary locally using cmake
        * ⚠️ Build binary using container
            * ✅ Dedicated container that uses volume
            * ⚠️ Dedicated container that uses github : blocked on github access.
        * Build docker image using multi-step build.
    * Orchestrated on K8s
    * Networking with DB
* 🎯 Browser-Client
    * Syncs DB state
    * Renders DB state
    * Works on mobile & on PC
## Development Environment
* VS Code
    * Explore if vscode is sufficent for debugging c code.
    * Add .vscode and files here, copy in as layer into image.
    * How to pull github on initial creation into the correct directory.
* VS
    * 🚧 Blocked on if VS code is sufficent.

# Systems

Each world has it's own systems. Worlds may share systems.


# Orchestration

Each world is it's own namespace within K8s. 

# Persistence

Each world can optionally be persistent. 
A persistent world will have it's own database and will have disaster-recovery.
A non-persistent world will not have disaster-recovery.

