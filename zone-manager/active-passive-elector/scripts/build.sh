#!/bin/bash

cmake -S /root/zone-manager/active-passive-elector -B /root/zone-manager/active-passive-elector/build
cd /root/zone-manager/active-passive-elector/build
make VERBOSE=1
cd /root/zone-manager