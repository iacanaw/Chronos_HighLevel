#!/bin/bash

# Parameters
XX=$1
YY=$2
SIM=$3

# copy data from the simulation folder
cp 'data/'$SIM'_SystemTemperature.tsv' 'scripts/temp/RAMP_'$SIM'/temp.log' 
cp 'data/'$SIM'_SystemPower.tsv' 'scripts/temp/RAMP_'$SIM'/power.log'

# changes the system size
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' 'scripts/temp/RAMP_'$SIM'/montecarlo.c'
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' 'scripts/temp/RAMP_'$SIM'/montecarlo.c'
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' 'scripts/temp/RAMP_'$SIM'/reliability.h'
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' 'scripts/temp/RAMP_'$SIM'/reliability.h'

cd 'scripts/temp/RAMP_'$SIM

# compile tools
g++ ramp_main.c reliability.cc -o ramp_main
gcc montecarlo.c -o montecarlo -lm

echo "Starting RAMP to generate statistics!"
# run the RAMP to generate the statistics
./ramp_main

cp 'montecarlofile' '../../../data/'$SIM'_montecarlofile'

echo "Starting MONTECARLO simulation to obtain the MTTF!"
# call the montecarlo to generate the system MTTF 
./montecarlo montecarlofile > '../../../data/'$SIM'_mttflog.txt'

rm montecarlo
rm ramp_main
rm montecarlofile
rm power.log
rm temp.log

# return to the base folder
cd ../..