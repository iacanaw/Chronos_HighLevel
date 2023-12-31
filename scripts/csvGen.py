import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 

folder_w = "GROUPED"
folder_p = "PATTERN"
folder_pid = "PIDTM"
folder_flea35 = "FLEA35"
folder_flea535 = "FLEA535"

folders = [folder_w, folder_p, folder_pid, folder_flea35, folder_flea535] 
labels = ["Grouped", "Pattern", "PIDTM", "FLEA35", "FLEA535"]
DIM_X = 20
DIM_Y = 20

with open("data/log.csv", "w") as logfile:
    pass

for folder in folders:
    print("Reading file " + folder + "...")
    temp_data = pd.read_csv("data/"+folder+"_SystemTemperature.tsv", sep='\t')
    temp_data = temp_data.to_numpy()

    power_data = pd.read_csv("data/"+folder+"_SystemPower.tsv", sep='\t')
    power_data = power_data.to_numpy()

    fit_data = pd.read_csv("data/"+folder+"_FITlog.tsv", sep='\t')
    fit_data = fit_data.to_numpy()

    temp_avg = 0
    temp_samples = 0

    peak_avg = 0
    peak_samples = 0

    fit_avg = 0
    fit_samples = 0

    power_avg = 0
    power_samples = 0

    for i in range (len(temp_data)):
        peak = 0
        for j in range(len(temp_data[i])-1):
            temp_avg += temp_data[i][j+1]
            temp_samples += 1
            if temp_data[i][j+1] > peak:
                peak = temp_data[i][j+1]
        peak_avg += peak
        peak_samples += 1

    peak_avg = peak_avg/peak_samples

    temp_avg = temp_avg/temp_samples

    for i in range (len(power_data)):
        for j in range(len(power_data[i])-1):
            power_avg += power_data[i][j+1]
            power_samples += 1

    power_avg = power_avg/power_samples

    for i in range (len(fit_data)):
        for j in range(len(fit_data[i])-1):
            fit_avg += fit_data[i][j+1]
            fit_samples += 1

    fit_avg = fit_avg/fit_samples

    with open("data/"+folder+"_mttflog.txt", "r") as mttffile:
        #line = mttffile.readline()
        line = mttffile.readline()
        splited = line.split(' ')
        mttf = splited[len(splited)-1]

    higherFIT = 0
    with open("data/"+folder+"_montecarlofile", "r") as montecarlofile:
        while(True):
            peFIT = 0
            for i in range(5):
                line = montecarlofile.readline()
                #print(line)
                if(line == ''):
                    break
                peFIT += float(line)
            if peFIT > higherFIT:
                higherFIT = peFIT
            if(line == ''):
                break
        mttf_higerFIT = (((1000000000/higherFIT)/24)/365)

    with open("data/"+folder+"_ThermalStatistics.csv", "r") as tsfile:
        line = tsfile.readline()
        splited = line.split(";")
        maxx = splited[0]
        maxx_std = splited[1]
        
        line = tsfile.readline()
        splited = line.split(";")
        q3 = splited[0]
        q3_std = splited[1]
        
        line = tsfile.readline()
        splited = line.split(";")
        median = splited[0]
        median_std = splited[1]
        
        line = tsfile.readline()
        splited = line.split(";")
        q1 = splited[0]
        q1_std = splited[1]
        
        line = tsfile.readline()
        splited = line.split(";")
        minn = splited[0]
        minn_std = splited[1]
        

    with open("data/log.csv", "a") as logfile:
        print(folder, file=logfile, end=';')
        print(str(DIM_X*DIM_Y), file=logfile, end=';')
        print(str(power_avg).replace(".","."), file=logfile, end=';')
        print(str(temp_avg).replace(".","."), file=logfile, end=';')
        print(str(peak_avg).replace(".","."), file=logfile, end=';')
        print(str(fit_avg).replace(".","."), file=logfile, end=';')
        print(str(mttf).replace(".",".").replace("\n", ""), file=logfile, end=';')
        print(str(higherFIT).replace(".","."), file=logfile, end=';')
        print(str(mttf_higerFIT).replace(".",".").replace("\n", ""), file=logfile, end=';')
        #print("-;-;-", file=logfile, end=';')
        print(maxx.replace(",","."), file=logfile, end=';')
        print(q3.replace(",","."), file=logfile, end=';')
        print(median.replace(",","."), file=logfile, end=';')
        print(q1.replace(",","."), file=logfile, end=';')
        print(minn.replace(",","."), file=logfile, end=';')
        print(maxx_std.replace("\n", "").replace(",","."), file=logfile, end=';')
        print(q3_std.replace("\n", "").replace(",","."), file=logfile, end=';')
        print(median_std.replace("\n", "").replace(",","."), file=logfile, end=';')
        print(q1_std.replace("\n", "").replace(",","."), file=logfile, end=';')
        print(minn_std.replace("\n", "").replace(",","."), file=logfile, end='\n')
        #print(clusterformation, file=logfile, end='\n')