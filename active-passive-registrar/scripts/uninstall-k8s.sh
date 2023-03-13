#!/bin/bash

kubectl delete pod world-simulator-service-registrar --grace-period=0

helm delete \
--namespace default \
world-simulator-service-registrar