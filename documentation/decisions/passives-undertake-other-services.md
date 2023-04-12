# Rationale

The architecture v2 stipulates that passives will undertake services by leveraging the data they already hold to provide HA fault tolerance to the active.

The result of this is:
+ Less network traffic : the active would need to provide data to servicers other than the passives
+ Less pods : having passives undertake services means other servicers need not exist
- More complex role handoff : if a passive becomes an active it needs to remove services
- More complex code base : numerous services in one code base

# Decision

tldr: One code base to one service relationship. Passives do not undertake any service. This results in less development complexity.

I think the more optimal and performant solution is to have passive's undertake services, and I think that I may end up with that. However during development and maturity of the code base and expertise in the technologies, I think having seperate services will result in a quicker and less complex development. 