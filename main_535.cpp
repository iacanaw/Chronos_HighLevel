#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include "reliability.h"
#include <math.h>

#define DIM_X 20
#define DIM_Y 20
#define N_STATES 35*5
#define N_TASKTYPE  3
#define SYSTEM_SIZE (DIM_X*DIM_Y)  
#define THERMAL_NODES (SYSTEM_SIZE*4)+12  // 4 thermal nodes for each PE plus 12 from the environment
#define TARGET_OCCUPATION 50
#define NUM_TASKS 38
#define LOG 0

unsigned int tableUpdates[N_TASKTYPE][N_STATES];

// pointer to read the tasks
FILE *ftasks = NULL;
FILE *powerlog = NULL;
FILE *fitlog = NULL;
FILE *fp = NULL;

struct Tasks{
    int id,type;
    float power;
    int totalTime, taskSlot, fit,temp, current_time;
};

Tasks many_core [DIM_Y][DIM_X];

// Tasks tasks [NUM_TASKS] =  {{0,2,0.350,1467,0},{1,1,0.242,1228,0},{2,0,0.190,607,0},{3,0,0.172,1999,0},
//                             {4,0,0.167,1223,0},{5,0,0.162,1399,0},{6,0,0.154,1409,0},{7,2,0.359,1138,0},
//                             {8,2,0.350,1603,0},{9,1,0.242,1835,0},{10,0,0.190,1243,0},{11,0,0.172,1756,0},
//                             {12,0,0.167,639,0},{13,0,0.162,547,0},{14,0,0.154,1073,0},{15,2,0.359,819,0}};

Tasks tasks [NUM_TASKS] =  {{0,2,0.350,467}, {1,1,0.242,228}, {2,0,0.190,607},  {3,0,0.172,999},
                            {4,0,0.167,223}, {5,0,0.162,399}, {6,0,0.154,409}, {7,2,0.359,138},
                            {8,2,0.472,603}, {9,0,0.135,835}, {10,0,0.151,243},{11,0,0.141,756},
                            {12,2,0.469,639}, {13,2,0.474,547}, {14,2,0.477,073},{15,2,0.459,819},
                            {16,1,0.271,479},{17,1,0.282,980},
                            {18,2,0.343,925},{19,2,0.362,669},{20,2,0.360,819}, {21,2,0.347,606},{22,2,0.364,930},{23,1,0.251,633},
                            {24,0,0.187,934}, {25,0,0.188,644}, {26,0,0.188,648}, {27,0,0.196,630}, {28,0,0.206,484},{29,0,0.116,610},{30,2,0.353,422},
                            {31,2,0.485,353},{32,2,0.382,776},{33,1,0.256,683},{34,1,0.326,371},{35,2,0.428,632},{36,1,0.263,910},{37,2,0.341,458}};


// Score table populated with high values!

float scoreTable[N_TASKTYPE][N_STATES];

struct floorplan_structure floorplan; 
extern struct UnitRel rel_unit[TOTAL_STRUCTURES];

double Binv[THERMAL_NODES][SYSTEM_SIZE];
double Cexp[THERMAL_NODES][THERMAL_NODES];

int power[DIM_Y][DIM_X];
double power_trace[SYSTEM_SIZE];
double t_steady[THERMAL_NODES];

double TempTraceEnd[THERMAL_NODES];
double Tsteady[THERMAL_NODES];
double Tdifference[THERMAL_NODES];
int SystemFIT[DIM_X*DIM_Y];

int state_last[DIM_X*DIM_Y], starting_fit[DIM_X*DIM_Y], state_stability[DIM_X*DIM_Y];
void FLEA_init(){
    for(int i = 0; i < SYSTEM_SIZE; i++){
        state_last[i] = -1;
        starting_fit[i] = -1;
        state_stability[i] = 0;
    }
}

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



//The place of x will be informed by sucessives sums and the y by num(addr%DIM_X or DIM_Y)
unsigned int API_getPEState(unsigned int id){
    unsigned int x = getX(id);
    unsigned int y = getY(id);
    int state_0, state_1, state, a;
    int d2, d1, d0, i0, i1, i2;

    unsigned int immediate[3], diagonal[3];
    
    for(a = 0; a < 3; a++){
        immediate[a] = 0;
        diagonal[a] = 0;
    }

    // SOUTH
    if(getSouth(x, y) != -1)
        immediate[getSouth(x,y)]++;
    // NORTH
    if(getNorth(x, y) != -1)
        immediate[getNorth(x,y)]++;
    // WEST
    if(getWest(x, y) != -1)
        immediate[getWest(x,y)]++;
    // EAST
    if(getEast(x, y) != -1)
        immediate[getEast(x,y)]++;

    // NORTH+EAST
    if(getNorthEast(x, y) != -1)
        diagonal[getNorthEast(x,y)]++;
    // NORTH+WEST
    if(getNorthWest(x, y) != -1)
        diagonal[getNorthWest(x,y)]++;
    // SOUTH+WEST
    if(getSouthWest(x, y) != -1)
        diagonal[getSouthWest(x,y)]++;
    // SOUTH+EAST
    if(getSouthEast(x, y) != -1)
        diagonal[getSouthEast(x,y)]++;
    
    i0 = immediate[0];
    i1 = immediate[1];
    i2 = immediate[2];

    d0 = diagonal[0];
    d1 = diagonal[1];
    d2 = diagonal[2];

    state_0 = (int)(i0 ? ((i0*i0*i0 - 18*i0*i0 + 107*i0) / 6) : 0);
    state_1 = (int)(i1 ? ((11*i1 - i1*i1 - 2*i0*i1) / 2) : 0);
    state = state_0 + state_1 + i2;

    if(d2 >= 2)
        state = state+35*1;
    else if(x == 0 || x == DIM_X-1 || y == 0 || y == DIM_Y-1)
        state = state+35*4;
    else if(d1 >= 2)
        state = state+35*2;
    else if(d0+d1 >= 3)
        state = state+35*3;
    
    if(state >= N_STATES){
        printf("ERRO CALCULANDO ESTADO: %d", state);
        while(1){}
    }
    return state;
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

int API_getMaxIdxfromRow(float scoreTable[N_TASKTYPE][N_STATES], unsigned int row){
    unsigned int max = 0, i;
    for( i = 1; i < N_STATES; i++){
        if(scoreTable[row][i] > scoreTable[row][max]){
            max = i;
        }
    }
    return max;
}

void API_PrintScoreTable(float scoreTable[N_TASKTYPE][N_STATES]){
    int i, j;
    FILE *fst, *fst2;
    fst = fopen("ScoreTable535.tsv", "w");
    fst2 = fopen("ScoreTable535_vector.tsv", "w");
    fprintf(fst2, "float scoreTable[N_TASKTYPE][N_STATES] = { ");
    for(i = 0; i < N_TASKTYPE; i++){
        fprintf(fst2, " {");
        for(j = 0; j < N_STATES; j++){
            fprintf(fst,"%d",(int)(scoreTable[i][j]*1000));
            fprintf(fst2,"%.1f",(float)((int)(scoreTable[i][j]*1000)));
            //fprintf(fst,"%d",(int)(scoreTable[i][j]*1000));
            if(j != N_STATES-1){
                fprintf(fst,"\t");
                fprintf(fst2,", ");
            }
        }
        fprintf(fst2, " }");
        if (i != N_TASKTYPE-1){
            fprintf(fst,"\n");
            fprintf(fst2,",\n");
        }
    }
    fprintf(fst2," };");
    fclose(fst);
    fclose(fst2);
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

void manyCorePrint(){ 
    FILE *fss;
    char tipo[3];
    fss = fopen("SystemShot535.tsv", "w");
    int id = 0;
    for(int j=0;j<DIM_Y;j++){
        for(int i=0;i<DIM_X;i++){
            if( many_core[j][i].type != -1 ) sprintf(tipo, "%d", many_core[j][i].type);
            else sprintf(tipo, "-");
            fprintf(fss,"(%2d,%2d){%s,%4d}\t",j, i, tipo, API_getPEState(id));
            id++;
        }
        fprintf(fss, "\n");
    }
    fclose(fss);
}

void printHeaders(){
    FILE *fl,*fpower,*fp;
#if LOG
    fl = fopen("data/FLEA_FITlog.tsv", "w");
    fp = fopen("data/FLEA_SystemTemperature.tsv", "w");
    fpower = fopen("data/FLEA_SystemPower.tsv", "w");
    fprintf(fp, "time");
    fprintf(fl, "time");
    fprintf(fpower, "time");
    for(int i=0;i<DIM_X*DIM_Y;i++){
        fprintf(fp, "\t%d",i);
        fprintf(fl, "\t%d",i);
        fprintf(fpower, "\t%d",i);
    }
    fprintf(fp, "\n");
    fprintf(fl, "\n");
    fprintf(fpower, "\n");
    fclose(fp);
    fclose(fl);
    fclose(fpower);
#endif
    return;
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

int getNextTask(){
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

void GROUPED_allocation(int task_to_allocate){
    int k = 0, slot;
    for(int i = 0; i < DIM_X; i++){
        for(int j = 0; j < DIM_Y; j++){
            slot = API_GetTaskSlotFromTile(k, task_to_allocate);
            k++;
            if (slot != -1){
                printf(" - Task %d allocated at addr: %dx%d", task_to_allocate, getY(k), getX(k));
                return;
            }
        }
    }
}

unsigned int getLessTested(int tasktype){
    unsigned int id, state, atualizacoes = 0, selected = 0;
    atualizacoes = 0xffffffff;
    for(id = 0; id < SYSTEM_SIZE; id++){
        if(many_core[getY(id)][getX(id)].type == -1){
            state = API_getPEState(id);
            if(atualizacoes >= tableUpdates[tasktype][state]){
                atualizacoes = tableUpdates[tasktype][state];
                selected = id;
            }
        }
    }
    return selected;
}

unsigned int getTotalUpdated(){
    unsigned int total = 0;
    for(int i = 0; i<N_TASKTYPE; i++){
        for(int j = 0; j<N_STATES; j++){
            total += tableUpdates[i][j];
        }
    }
    return total;
}

void FLEA_allocation(int task_to_allocate){
    int sorted_id[DIM_X*DIM_Y], sorted_score[DIM_X*DIM_Y];
    int k = 0;
    for(int i = 0; i < DIM_X; i++){
        for(int j = 0; j < DIM_Y; j++){
            sorted_id[k] = k;
            sorted_score[k] = (int)(scoreTable[tasks[task_to_allocate].type][API_getPEState(k)]*1000);
            k++;
        }
    }
    k=0;
    quickSort(sorted_score, sorted_id, 0, (DIM_X*DIM_Y)-1);
    int id, slot = -1;
    for(int i = SYSTEM_SIZE-1; i >= 0; i--){
        id = sorted_id[i];
        slot = API_GetTaskSlotFromTile(id, task_to_allocate);
        if (slot != -1) break;
    }
    if (slot != -1)
        printf(" - Task %d allocated at addr: %dx%d", task_to_allocate, getY(id), getX(id));
    else{
        printf(" - Task %d not allocated!!:\n", task_to_allocate); while(1){}
    }
}


void FLEA_training_allocation(int task_to_allocate){
    float epsilon = 0.2;
    int sorted_id[DIM_X*DIM_Y], sorted_score[DIM_X*DIM_Y];
    int k = 0;
    for(int i = 0; i < DIM_X; i++){
        for(int j = 0; j < DIM_Y; j++){
            sorted_id[k] = k;
            sorted_score[k] = (int)(scoreTable[tasks[task_to_allocate].type][API_getPEState(k)]*1000);
            k++;
        }
    }
    k=0;
    quickSort(sorted_score, sorted_id, 0, (DIM_X*DIM_Y)-1);
    int id, slot = -1;
    if((int)(epsilon*100) > random()%100){ 
        // int oneofthree = random()%3;
        // if(oneofthree != 0){
            id = getLessTested(tasks[task_to_allocate].type); // try the less tested state available
            printf(" - %d LESS", API_getPEState(id));
            slot = API_GetTaskSlotFromTile(id, task_to_allocate);
        // } else{
        //     printf(" - RAND");
        //     do{
        //         id = sorted_id[random()%SYSTEM_SIZE];
        //         slot = API_GetTaskSlotFromTile(id, task_to_allocate);
        //     }while(slot == -1);
        // }
    } else{ // uses the learned information
        // try to get the best tile slot
        for(int i = (DIM_X*DIM_Y)-1; i >= 0; i--){
            id = sorted_id[i];
            slot = API_GetTaskSlotFromTile(id, task_to_allocate);
            if (slot != -1) break;
        }
    }
    if (slot != -1)
        printf(" - Task %d allocated at addr: %dx%d", task_to_allocate, getY(id), getX(id));
}

// float Q_rsqrt( float number ){
//     long i;
//     float x2, y;
//     const float threehalfs = 1.5F;

//     x2 = number * 0.5F;
//     y  = number;
//     i  = * ( long * ) &y;                       // evil floating point bit level hacking
//     i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
//     y  = * ( float * ) &i;
//     y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//     y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

//     return y;
// }

void FLEA_training(int time){
    int addr=0, slot=0, taskType=0, current_fit=0, toprint=0, state, tp,  maxid;
    
    //---------------------------------------
    // --------- Q-learning stuff -----------
    // Hyperparameters
    //float discount = (float) time / 10000000.0;
    float alpha = 0.01;// - (0.01*discount);
    float gamma = 0.6;
    float oldvalue, maxval, reward = 0.0,  delta = 0.0;

    FILE *freward;
    freward = fopen("Rewards535.log", "a");
    
    for(int i = 0; i < SYSTEM_SIZE; i++){
        taskType = many_core[getY(i)][getX(i)].type;
        current_fit = many_core[getY(i)][getX(i)].fit;
        if(taskType != -1){ 
            state = API_getPEState(i);
            
            
            //printf("state: %d", state);
            // if the state has changed
            if(state != state_last[i]){ 
                state_last[i] = state;
                starting_fit[i] = many_core[getY(i)][getX(i)].fit;
                state_stability[i] = 0;
            } 
            else if ( state_stability[i] >= 300 ) {
            // calculates the reward

                delta = ((float)(current_fit/100) - (float)(starting_fit[i]/100))*0.003;
                reward = 100.0+(delta*delta);
                //reward = Q_rsqrt(reward);
                reward = 1/sqrt(reward);
                reward = ((-400.0 * reward * delta)+50.0);
                // if(delta >= 2.0 && reward > 0.0){
                //     printf("\nErro no cálculo do reward!\nDelta: %f; Reward: %f\n", delta, reward);
                //     throw 3;
                // }
                fprintf(freward, "%.4f; %.4f\n", delta, reward);
                 
                // printf("Delta: %.2f -- Current: %.2f Starting: %.2f \n",delta, (float)(current_fit/100),(float)(starting_fit[i]/100));
                // printf("Reward: %.2f\n",  reward);

                // gets the old value
                oldvalue = scoreTable[taskType][state];
                
                // gets the max value from the table
                //maxid = API_getMaxIdxfromRow(&(scoreTable[0][0]), taskType, N_STATES, N_TASKTYPE);
                maxid = API_getMaxIdxfromRow(scoreTable, taskType);
                maxval = scoreTable[taskType][maxid];
                // printf("MaxID: %d -- MaxVal: %.2f\n", maxid, maxval);

                // updates the score table
                scoreTable[taskType][state] = (((1 - alpha) * oldvalue) + (alpha * ( reward + (gamma * maxval))));
                tableUpdates[taskType][state]++;

                // saves the current FIT for the next update
                state_stability[i] = 100;
                starting_fit[i] = current_fit;
            }
            else{
                state_stability[i]++;
            }
        }
    }
    fclose(freward);
    // print score table
    if(time % 100 == 0) API_PrintScoreTable(scoreTable);
    printf(" - Updates so far: %ld", getTotalUpdated());
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

int main(int argc, char *argv[]){

    srand(time(0));

    // leitura dos arquivos binv e cexp do TEA
    load_matrices();

    if(powerlog == NULL){
        powerlog = fopen("data/FLEA_SystemPower.tsv", "a");
    }
    if(fp == NULL){
        fp = fopen("data/FLEA_SystemTemperature.tsv", "a");
    }  
    if(fitlog == NULL){
        fitlog = fopen("data/FLEA_FITlog.tsv", "a");
    }

    // manycore model initializing
    for(int i=0;i<DIM_X;++i){
        for(int j=0;j<DIM_Y;++j){
            many_core[j][i].id = 0;
            many_core[j][i].type = -1;
            many_core[j][i].power = 0.1;
            many_core[j][i].totalTime = -1;
            many_core[j][i].taskSlot = 1;
            many_core[j][i].current_time = 0;
            many_core[j][i].fit = 1000;
            many_core[j][i].temp = 0;
        }
    }

    printHeaders();

    int cont=0, allocate_task=-1;
    
    /* FLEA INITIALIZATION */
    FLEA_init();
    for(int i = 0; i < N_TASKTYPE; i++){
        for(int j = 0; j < N_STATES; j++ ){
            //scoreTable[i][j] = scoreTable[i][j] - scoreTable[i][j] + (float)random()%2000;
            //scoreTable[i][j] = 1000.0;
            //scoreTable[i][j] = 0.0;
            tableUpdates[i][j] = 0;
            // if( scoreTable[i][j] == 0.0 ) tableUpdates[i][j] = 0;
            // else tableUpdates[i][j] = 1000;
            scoreTable[i][j] = 0.0;
        }
    }
    
    while(1){
        int barra_ene = 0;    
        printf("\rTime: %.3fs - SysOc: %d",((float)cont/1000), getOccupation());
        cont++;

        // updates the temperature and FIT
        calcula_temp();
        calcula_fit();   

        // run until 1 sec of simulation
        if(cont == 60000000){
            break; 
        } else if( cont % 1000 == 0 ){
            manyCorePrint();
        }
        // STARTING SIMULATION...
        // if the time is over 20 ms start to evaluate the system
        else if(cont>20){
            
            // checks if the system is running at target occupation
            if( getOccupation() < int(20*sin(cont/100000) + TARGET_OCCUPATION)  ){
                allocate_task = getNextTask();
                
                if(allocate_task != -1){
                    FLEA_training_allocation(allocate_task); barra_ene = 1;
                    //FLEA_allocation(allocate_task); barra_ene = 1;
                }
            }
            if(cont > 1000) FLEA_training(cont);
        }

        // write the time into the log files:
#if LOG
        fprintf(fitlog, "%.5f", ((float)cont/1000));
        fprintf(powerlog, "%.5f", ((float)cont/1000));
        fprintf(fp, "%.5f", ((float)cont/1000));
#endif
        for(int i=0;i<SYSTEM_SIZE;++i){
#if LOG
            // write info into the log files
            fprintf(powerlog,"\t%f",power_trace[i]);
            fprintf(fp, "\t%.2f", (((float)(TempTraceEnd[i]*100)/100)-273.15));
            fprintf(fitlog,"\t%f",rel_unit[i].ind_inst);
#endif  
            if(many_core[getY(i)][getX(i)].type != -1) { 
                // checks if the task has finished
                if(many_core[getY(i)][getX(i)].current_time >= many_core[getY(i)][getX(i)].totalTime) {
                    printf(" - Task %d dealocated from addr %dx%d",many_core[getY(i)][getX(i)].id,(int)i/DIM_X, i%DIM_X ); barra_ene = 1;
                    //manyCorePrint();
                    //getchar();
                    many_core[getY(i)][getX(i)].id = 0;
                    many_core[getY(i)][getX(i)].type = -1;
                    many_core[getY(i)][getX(i)].power = 0.1;
                    many_core[getY(i)][getX(i)].totalTime = -1;
                    many_core[getY(i)][getX(i)].current_time = 0;
                    many_core[getY(i)][getX(i)].taskSlot = 1;
                    many_core[getY(i)][getX(i)].type = -1;
                }
                else{
                    // increase the task running counter 
                    many_core[getY(i)][getX(i)].current_time++;
                }
            }
        }
#if LOG
        fprintf(fitlog,     "\n");
        fprintf(powerlog,   "\n");
        fprintf(fp,         "\n");
#endif
        if(barra_ene) printf("\n");
    }
    return 0;
}