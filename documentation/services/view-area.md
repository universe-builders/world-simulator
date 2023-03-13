# Responsibility
A persistent connection connects and provides a filter of component-types and area. This service provides all the entity's in that area with their components specified by the component-types. Any entity without any of the component-types is filtered out.

# Strat

* uses discover-zones to get all zones within an area for specified LayerType's.
* connects to views for each of the zones.
* continually sends the view to the viewer.