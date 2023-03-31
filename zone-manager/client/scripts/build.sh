#!/bin/bash

cmake -S /root/zone-manager/client -B /root/zone-manager/client/built
cd /root/zone-manager/client/built
make VERBOSE=1
cd /root/zone-manager/client