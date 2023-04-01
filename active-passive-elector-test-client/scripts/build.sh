#!/bin/bash

cmake -S /root/active-passive-elector-test-client -B /root/active-passive-elector-test-client/built
cd /root/active-passive-elector-test-client/built
make VERBOSE=1
cd /root/active-passive-elector-test-client
