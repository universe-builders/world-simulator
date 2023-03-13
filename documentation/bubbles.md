# Rationale

It would be extremely inefficent and bad experience for a group to be split between a boundary on a layer. The current way I am thinking of handling groups is to have areas be explicitly defined, such as a boss fight area, so that the group will not be split. 

Another way could be the original SpatialOS way (i cannot remember why they changed) which is to have a 'bubble' around a group.

This bubble method I think works well for creating a high LOD simulation, whereas the current architecture could be used for the medium or low LOD simulations.

A bubble would follow a group around and encase it - if a player runs off by themselves then the bubble is expanded until eventually it is seperated and now two bubbles exist. Only the areas that contain players will be in a bubble and thus will have high LOD simulation. If the world was quite dense then this might end up being the entire world being in a bubble, at which point the original architecture would be fine, but I think right now the world will be sparse enough that a lot of it will not be in a bubble, and be on low LOD.

? Do i want to consider the LODing system now or leave that for later 
    * I think the LODing system would integrate with the current architecture as it stands right now, I don't think it will require a massive overhaul of architecture. Also I think that the bubble system will integrate well. For now I want to get the current simulation system up and running and then I will transition to this sytem with LODing as required.

# Ideas

* Would the logic be different between LOD's?
    * if so: then this system may work
        ** the fidelity of the simulation should definitely decrease, eg instead of having a wolf do a charge attack it should just deal X damage based on a roll
        ** some entites don't get simulated at all in low-res
    * if not: can I architect my systems in a way that can step in any frequency so that I can write once and then a bubble just increases the frequency of updates for units? This could be simpler.

* Bubbles shouldn't be horizontally scaled, instead should be vertically scaled. All pods should ideally exist on fewest number of nodes, ideally on one so that networking has low latency and bandwidth.