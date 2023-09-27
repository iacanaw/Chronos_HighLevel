#!/bin/bash

for occupation in 70 50 30; do
    ./scripts/RunTests.sh $occupation 60
done
