# Rationale
For services to maintain high-availability a common architectural pattern is to use N replicas in active-passive roles. The active is the 'leader' and will be the service provider. Passives will sync state with the active and will continously check if active role is available, in the case that active died, and undertake that role. 

# Responsibilities
* Allow clients to claim a lease and become sole active.
* Allow clients to await lease expiry and become sole active.
* Allow clients to claim role of passive until able to become sole active.

# Usage
Used as a side car in a pod to provide active or passive role.