#!/bin/bash

OCC=$1
TIME=$2

sed -i 's/#define SIM_SEC.*/#define SIM_SEC '$TIME'/' utils.h
sed -i 's/#define TARGET_OCCUPATION.*/#define TARGET_OCCUPATION '$OCC'/' utils.h

for sim in 'PATTERN' 'GROUPED' 'FLEA35' 'FLEA535' 'PIDTM'; do
    echo $sim
    g++ 'main_'$sim'.cpp' reliability.c utils.cpp -o 'app_'$sim
    './app_'$sim &
done

wait

python3 scripts/thermal_shot.py &

for sim in 'PATTERN' 'GROUPED' 'FLEA35' 'FLEA535' 'PIDTM'; do
    python3 scripts/graphTemperature.py $sim &
    cp -r scripts/RAMP 'scripts/temp/RAMP_'$sim
    ./scripts/RunRAMP.sh 20 20 $sim &
done

wait

for sim in 'PATTERN' 'GROUPED' 'FLEA35' 'FLEA535' 'PIDTM'; do
    rm -rf 'scripts/temp/RAMP_'$sim
done

#python3 scripts/thermal_shot.py
mv data/thermal_shot.png 'data/ThermalShotOcc'$OCC'Time'$TIME'sec.png'
python3 scripts/csvGen.py
mkdir -p 'data/ResultOcc'$OCC'Time'$TIME'sec'
mv data/*_* 'data/ResultOcc'$OCC'Time'$TIME'sec/'
mv data/log.csv 'data/logOcc'$OCC'Time'$TIME'sec.csv'

