#!/bin/bash

# Parameters
XX=$1
YY=$2
SIM=$3

# copy data from the simulation folder
cp 'data/'$SIM'_SystemTemperature.tsv' 'scripts/RAMP/temp.log'
cp 'data/'$SIM'_SystemPower.tsv' 'scripts/RAMP/power.log'

# changes the system size
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' scripts/RAMP/montecarlo.c
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' scripts/RAMP/montecarlo.c  
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' scripts/RAMP/reliability.h
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' scripts/RAMP/reliability.h

cd scripts/RAMP

# compile tools
g++ ramp_main.c reliability.cc -o ramp_main
gcc montecarlo.c -o montecarlo -lm

echo "Starting RAMP to generate statistics!"
# run the RAMP to generate the statistics
./ramp_main

cp 'montecarlofile' '../../data/'$SIM'_montecarlofile'

echo "Starting MONTECARLO simulation to obtain the MTTF!"
# call the montecarlo to generate the system MTTF 
./montecarlo montecarlofile > '../../data/'$SIM'_mttflog.txt'

# return to the base folder
cd ../..