#!/bin/bash

cmake -S /root/zone-manager/active-passive-elector -B /root/zone-manager/active-passive-elector/built
cd /root/zone-manager/active-passive-elector/built
make VERBOSE=1
cd /root/zone-manager