#!/bin/bash

cmake -S /root/distribute-zone-simulation -B /root/distribute-zone-simulation/built
cd /root/distribute-zone-simulation/built
make VERBOSE=1
cd /root/distribute-zone-simulation
