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

for sim in 'PATTERN' 'GROUPED' 'FLEA35' 'FLEA535' 'PIDTM'; do
    python3 scripts/graphTemperature.py $sim &
    ./scripts/RunRAMP.sh 20 20 $sim
done

wait

python3 scripts/thermal_shot.py
python3 scripts/csvGen.py
mkdir 'data/ResultOcc'$OCC'_'$TIME'sec'
mv data/*_* ' data/ResultOcc'$OCC'_'$TIME'sec'
mv data/log.csv 'data/log_'$OCC'_'$TIME'sec.csv'

