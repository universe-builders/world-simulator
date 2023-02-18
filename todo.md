# To do

* Should each world have a unique set of systems?

* Should worlds be created like creating a scene, playing, and then iterating or should worlds be created like they were living breathing things?
    ** I see sense in having a declared default state for a world for reproducability and performance. Perhaps you can create an image, a snapshot, of the world at a certain point and then use that as a base.
    ** I like the idea of it being editing a live breathing world and I think that the logic should account for that.

* How do I allow for entities to have lives at different fidelities? I don't want to continually simulate all entities at high fidelity, for the sake of resource utilization. How do they move between fidelities? How does a player leave an area and come back and it look like the entities there were carrying out their initial actions (wolf chasing rabbit, ..)?