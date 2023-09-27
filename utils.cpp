#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include <math.h>
#include "utils.h"
#include "reliability.h"

// Tarefas
Tasks tasks[NUM_TASKS] =  {{0,2,0.350,467}, {1,1,0.242,228}, {2,0,0.190,607},  {3,0,0.172,999},
                            {4,0,0.167,223}, {5,0,0.162,399}, {6,0,0.154,409}, {7,2,0.359,138},
                            {8,2,0.472,603}, {9,0,0.135,835}, {10,0,0.151,243},{11,0,0.141,756},
                            {12,2,0.469,639}, {13,2,0.474,547}, {14,2,0.477,073},{15,2,0.459,819},
                            {16,1,0.271,479},{17,1,0.282,980},
                            {18,2,0.343,925},{19,2,0.362,669},{20,2,0.360,819}, {21,2,0.347,606},{22,2,0.364,930},{23,1,0.251,633},
                            {24,0,0.187,934}, {25,0,0.188,644}, {26,0,0.188,648}, {27,0,0.196,630}, {28,0,0.206,484},{29,0,0.116,610},{30,2,0.353,422},
                            {31,2,0.485,353},{32,2,0.382,776},{33,1,0.256,683},{34,1,0.326,371},{35,2,0.428,632},{36,1,0.263,910},{37,2,0.341,458}};

// Sistema
Tasks many_core[DIM_Y][DIM_X];

// Temperature and Reliability stuff
double Binv[THERMAL_NODES][SYSTEM_SIZE];
double Cexp[THERMAL_NODES][THERMAL_NODES];
int power[DIM_Y][DIM_X];
double power_trace[SYSTEM_SIZE];
double t_steady[THERMAL_NODES];
double TempTraceEnd[THERMAL_NODES];
double Tsteady[THERMAL_NODES];
double Tdifference[THERMAL_NODES];
int SystemFIT[DIM_X*DIM_Y];
struct floorplan_structure floorplan; 

int getX(int id){
    int retorno = id%DIM_X;
    if(retorno >= DIM_X || retorno < 0){
        printf("\nErro getX(%d) = %d\n", id, retorno);
        throw 1;
    }
    return retorno;
}

int getY(int id){
    int retorno = (int)(id/DIM_Y);
    if(retorno >= DIM_Y || retorno < 0){
        printf("\nErro getY(%d) = %d\n", id, retorno);
        throw 1;
    } 
    return retorno;
}



int getSouth(int x, int y){
    if(y > 0){
        return(many_core[y-1][x].type);
    } else {
        return(-1);
    }
}

int getNorth(int x, int y){
    if(y < DIM_Y-1){
        return(many_core[y+1][x].type);
    } else {
        return(-1);
    }
}

int getEast(int x, int y){
    if(x < DIM_X-1){
        return(many_core[y][x+1].type);
    } else {
        return(-1);
    }
}

int getWest(int x, int y){
    if(x > 0){
        return(many_core[y][x-1].type);
    } else {
        return(-1);
    }
}


int getNorthEast(int x, int y){
    if(y < DIM_Y-1 && x < DIM_X-1){
        return(many_core[y+1][x+1].type);
    } else {
        return(-1);
    }
}

int getNorthWest(int x, int y){
    if(y < DIM_Y-1 && x > 0){
        return(many_core[y+1][x-1].type);
    } else {
        return(-1);
    }
}

int getSouthWest(int x, int y){
    if(y > 0 && x > 0){
        return(many_core[y-1][x-1].type);
    } else {
        return(-1);
    }
}

int getSouthEast(int x, int y){
    if(y > 0 && x < DIM_X-1){
        return(many_core[y-1][x+1].type);
    } else {
        return(-1);
    }
}

// A utility function to swap two elements 
void swap(int* a, int* b) { 
    int t = *a; 
    *a = *b; 
    *b = t; 
}


/* This function takes last element as pivot, places 
the pivot element at its correct position in sorted 
array, and places all smaller (smaller than pivot) 
to left of pivot and all greater elements to right 
of pivot */
int partition (int arr[], int arr2[], int low, int high) { 
    int pivot = arr[high]; // pivot 
    int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far
    int j;
    for (j = low; j <= (high - 1); j++) { 
        // If current element is smaller than the pivot 
        if (arr[j] < pivot) { 
            i++; // increment index of smaller element 
            swap(&arr[i], &arr[j]);
            swap(&arr2[i], &arr2[j]);
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    swap(&arr2[i + 1], &arr2[high]); 
    return (i + 1); 
} 

/* The main function that implements QuickSort 
arr[] --> Array to be sorted, 
low --> Starting index, 
high --> Ending index */
void quickSort(int arr[], int arr2[], int low, int high){ 
    if (low < high){ 
        /* pi is partitioning index, arr[p] is now 
        at right place */
        int pi = partition(arr, arr2, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, arr2, low, (pi - 1)); 
        quickSort(arr, arr2, (pi + 1), high); 
    }
}

void randPositions(int arr[], int arr2[], int low, int high){
    for(int i = low; i < (high-1); i++){
        if(arr[i] == arr[i+1]){
            if( 60 > random()%100 ){
                swap(&arr[i], &arr[i+1]);
                swap(&arr2[i], &arr2[i+1]);
            }
        }
    }
}

unsigned int API_GetTaskSlotFromTile(unsigned int id, unsigned int app){
    if(many_core[getY(id)][getX(id)].taskSlot > 0){
        many_core[getY(id)][getX(id)].taskSlot = many_core[getY(id)][getX(id)].taskSlot - 1;
        if(many_core[getY(id)][getX(id)].type == -1){
            many_core[getY(id)][getX(id)].id = tasks[app].id;
            many_core[getY(id)][getX(id)].type = tasks[app].type;
            many_core[getY(id)][getX(id)].power = tasks[app].power;
            many_core[getY(id)][getX(id)].totalTime = tasks[app].totalTime;
            many_core[getY(id)][getX(id)].current_time = 0;
        }
        return 1;
    }else {
        return -1;
    }
}

void load_matrices(){
    FILE *binvpointer;
    binvpointer = fopen("20x20/binv.txt","r");
    FILE *cexppointer;
    cexppointer = fopen("20x20/cexp.txt","r");

    char line[1200000];
    char *number;
    int column, row;

    //printf("atira4");

    for (row = 0; row < THERMAL_NODES; row++){
        fgets(line, sizeof(line), binvpointer);
        number = strtok(line, " ");
        for(column = 0; column < SYSTEM_SIZE; column++){
            Binv[row][column] = strtod(number, NULL);
            //printf("%f ", Binv[row][column]); 
            number = strtok(NULL, " ");      
        }
    }


    for (row = 0; row < THERMAL_NODES; row++){
        fgets(line, sizeof(line), cexppointer);
        number = strtok(line, " ");
        for(column = 0; column < THERMAL_NODES; column++){
            Cexp[row][column] = strtod(number, NULL);
            //printf("%f ", Cexp[row][column]); 
            number = strtok(NULL, " ");      
        }
    }

    fclose(binvpointer);
    fclose(cexppointer);

    for(int i=0;i<THERMAL_NODES;i++){
        TempTraceEnd[i] = 313.15; // Kelvin
    }

    int unitc;
	for (unitc = 0; unitc < TOTAL_STRUCTURES; unitc++){
        sprintf(floorplan.units[unitc].name, "p%d", unitc);
        floorplan.units[unitc].height = 0.000194; // mem 8Kb
        floorplan.units[unitc].width = 0.000194; // mem 8Kb

        init(&floorplan, unitc);  /* Initialize structures*/
        fitinit(unitc);           /* Initialize FITS for each structure*/
    }
}

void calcula_temp(){ 
    int index = 0;
    // estimate the power consumption of each PE
    for (int yi = 0; yi < DIM_Y; yi++){
	    for(int xi = 0; xi < DIM_X; xi++){
		    float variation = (float)(rand()%30) - 15.0; // variates 15%
            variation = variation/100;
            if(many_core[yi][xi].type != -1){
                power_trace[index] = many_core[yi][xi].power + (many_core[yi][xi].power*variation);
                if (power_trace[index] < 0.1) 
                    power_trace[index] = 0.1;
            }
            else{
                power_trace[index] = 0.1;
            }
		    index++;
        }
	}

    // calculates the temperature of each PE
    int i, j;
    double heatContributionPower;

    for(i = 0; i < THERMAL_NODES; i++){
        heatContributionPower = 0;
        for(j = 0; j < SYSTEM_SIZE; j++){
            heatContributionPower += Binv[i][j]*power_trace[j];
        }
        Tsteady[i] = heatContributionPower + 318.15; // soma com Temperatura Ambiente
        Tdifference[i] = TempTraceEnd[i] - Tsteady[i]; // Iaçanã: trouxe pra cá esse calculo, estava em outro for()
    }

    //for(int k = 0; k < THERMAL_NODES; k++) Tdifference[k] = TempTraceEnd[k] - Tsteady[k];    

    for(int k = 0; k < THERMAL_NODES; k++){
        double sumExponentials = 0;
        for(j = 0; j < THERMAL_NODES; j++){
            sumExponentials += Cexp[k][j] * Tdifference[j];
        }
        TempTraceEnd[k] = Tsteady[k] + sumExponentials;
    }
}

void calcula_fit(){ 
    for (int structures=0; structures < TOTAL_STRUCTURES; structures++){
        /* Calculate FIT value by feeding in each structures temperature, activity
            * factor, processor supply voltage, and processor frequency. */
        //printf("Info: Temp : %f , Power: %f, strct = %d",TempTraceEnd[structures], power_trace[structures],structures);
        allmodels(TempTraceEnd[structures], power_trace[structures], 1.0,  1.0,  structures);
	}

    //for(int i = 0; i < DIM_Y*DIM_X; i++) {}SystemFIT[i] = (int)rel_unit[i].ind_inst*100;

    int m, n, i = 0, avgFit = 0, totalFit = 0;
    for (n = 0; n < DIM_Y; n++){
        for (m = 0; m < DIM_X; m++){
            SystemFIT[i] = (int)rel_unit[i].ind_inst*100;
            avgFit = many_core[n][m].fit;
            // printsv("avgFit ", avgFit);
            totalFit = many_core[n][m].fit << 5;
            // printsv("totalFit1 ", totalFit);
            totalFit = totalFit - avgFit;
            // printsv("totalFit2 ", totalFit);
            totalFit = totalFit + SystemFIT[i];
            // printsv("totalFit3 ", totalFit);
            many_core[n][m].fit = totalFit >> 5;
            //printf("FIT %d => %d\n",i, many_core[n][m].fit);
            i++;
        }
    }
}

int readline(FILE *f, char *buffer, size_t len){
    char c; 
    int i;
    memset(buffer, 0, len);
    for (i = 0; i < len; i++){   
        int c = fgetc(f); 
        if (!feof(f)) {   
            if (c == '\r')
                buffer[i] = 0;
            else if (c == '\n'){   
                buffer[i] = 0;
                return i+1;
            }   
            else
            buffer[i] = c; 
        }   
        else{   
            return -1; 
        }   
    }   
    return -1; 
}

int getNextTask(FILE *ftasks){
    while(ftasks == NULL){
        ftasks = fopen("tasks", "r");
        printf("\nTasks file is open!\n");
    }
    char task[4];
    int task_i;
    readline(ftasks, task, 4);
    task_i = atoi(task);
    if(task_i == -1){
        fclose(ftasks);
    }
    return task_i;
}

int getOccupation(){
    int cont = 0;
    for(int i = 0; i<DIM_X; i++){
        for(int j = 0; j < DIM_Y; j++){
            if(many_core[j][i].type != -1){
                cont++;
            }
        }
    }
    return ((cont*100)/SYSTEM_SIZE);
}

int tagetOccupation(int cont){
    //return (int)(20*sin(cont/10000) + TARGET_OCCUPATION);
    return (int)(TARGET_OCCUPATION);
}