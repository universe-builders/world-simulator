# Responsibilities
* Provides a mechanism for processes to be assigned an active or passive role, based on their success on getting a lease.

# Context
* K8s uses the lease resource to allow services to lock resources for workload distribution, such as two DB replicas where one is active (performing mutations) and one is passive as a backup for HA.
See: https://kubernetes.io/docs/concepts/architecture/leases/

# Usage
* Servicers can connect via TCP to this process, provide an identity and lease information, and this service will assign a role based on the ability to obtain that lease. The lease will continually be attempted to be obtained. The servicer can use this information to access resources, such as a service, based on their role.