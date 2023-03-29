#!/bin/bash

cmake -S /root/zone-manager -B /root/zone-manager/build
cd /root/zone-manager/build
make VERBOSE=1
cd /root/zone-manager