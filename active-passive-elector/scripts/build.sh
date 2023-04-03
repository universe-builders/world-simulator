#!/bin/bash

cmake -S /root/active-passive-elector -B /root/active-passive-elector/built
cd /root/active-passive-elector/built
make VERBOSE=1
cd /root/active-passive-elector
