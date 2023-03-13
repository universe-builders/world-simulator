# Requirements

* Service checklist (⭕ | ⚠️ | ✅)
* ⭕ Responsibilities defined
* ⭕ Responsibility strategies defined
* ⭕ Strategies defined for K8s and Web
* ⭕ Replica oriented
* ⭕ Fault Tolerant
* ⭕ Disaster Recovery Strategy defined
* ⭕ Graceful migration on termination
* ⭕ Horizontally auto scalable
* ⭕ Rolling updatable
* ⭕ Channels defined
* ⭕ Events defined
* ⭕ Workflows defined

* Each responsibility should have it's own discoverability.

* Should be able to run in single area mode, including on web client only, ideally without having to specify a mode -- it just shouldn't orchestrate certain components.
    * component-authority-issuer is the only one that needs to be changed, could potentially have a query of the number of nodes that a world can be orchestrated on.