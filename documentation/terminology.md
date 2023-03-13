# Area
An 2D area within the world which may have entities within.

Area:
    {
        bounds: Rect
    }

# LayerType
A layer type is used to identify a layer as seperate from other layers. It is defined by a set of component types which can be operated on in isolation.

LayerType:
    {
        componentTypes: string[]
    }

# Simulator
A simulator simulates the component-set defined in the layer for the area defined by the zone.

Simulator:
    {
        layerType: LayerType
        zone: Zone
    }

# Zone
A zone is a defined area which is simulated by a simulator that handles that layerType. Zones only have one layerType.

Zone:
    {
        layerType: LayerType
        area: Area
        simulator: Simulator
    }

# Layer
A layer is an infinite area constrained to a single layer with a number of defined zones which are being simulated.

Layer:
    {
        layerType: LayerType
        zones: Zone[]
    }

# World
A world is a stack of layers that all operate seperately and in parallel. These layers may communicate amongst each other.

World:
    {
        layers: LayerType->Layer
    }