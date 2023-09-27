#!/bin/bash

for occupation in 90 70 50 30; do
    ./scripts/RunTests.sh $occupation 60
done
