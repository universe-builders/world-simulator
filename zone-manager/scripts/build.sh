#!/bin/bash

cmake -S /root/zone-manager -B /root/build
cd /root/build
make
cd /root/zone-manager