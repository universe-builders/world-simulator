#!/bin/bash

cmake -S /root/zone-manager -B /root/build
cd /root/build
make VERBOSE=1
cd /root/zone-manager