#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include "reliability.h"

#define DIM_X 20
#define DIM_Y 20
#define N_STATES 35
#define N_TASKTYPE  3
#define SYSTEM_SIZE (DIM_X*DIM_Y)  
#define THERMAL_NODES (SYSTEM_SIZE*4)+12  // 4 thermal nodes for each PE plus 12 from the environment
#define TARGET_OCCUPATION 70
#define NUM_TASKS 38
// PIDTM defines
#define KP 10
#define KI 10
#define KD 10
#define INT_WINDOW 10

// PIDTM stuff
volatile unsigned int measuredWindows;
typedef struct{
    // PID control variables
    unsigned int derivative[DIM_Y * DIM_X];
    unsigned int integral[DIM_Y * DIM_X];
    unsigned int integral_prev[INT_WINDOW][DIM_Y * DIM_X];
    unsigned int Temperature_prev[DIM_Y * DIM_X];
    unsigned int control_signal[DIM_Y * DIM_X];
} PIDinfo;
volatile PIDinfo pidStatus;


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

Tasks tasks [NUM_TASKS] =  {{0,2,0.350,1467}, {1,1,0.242,1228}, {2,0,0.190,607},  {3,0,0.172,1999},
                            {4,0,0.167,1223}, {5,0,0.162,1399}, {6,0,0.154,1409}, {7,2,0.359,1138},
                            {8,2,0.472,1603}, {9,0,0.135,1835}, {10,0,0.151,1243},{11,0,0.141,1756},
                            {12,2,0.469,639}, {13,2,0.474,547}, {14,2,0.477,1073},{15,2,0.459,819},
                            {16,1,0.271,1479},{17,1,0.282,980},
                            {18,2,0.343,1925},{19,2,0.362,1669},{20,2,0.360,819}, {21,2,0.347,1606},{22,2,0.364,1930},{23,1,0.251,1633},
                            {24,0,0.187,934}, {25,0,0.188,644}, {26,0,0.188,648}, {27,0,0.196,630}, {28,0,0.206,1484},{29,0,0.116,1610},{30,2,0.353,1422},
                            {31,2,0.485,1353},{32,2,0.382,776},{33,1,0.256,683},{34,1,0.326,1371},{35,2,0.428,1632},{36,1,0.263,1910},{37,2,0.341,1458}};

/* PATTERN STUFF */
unsigned int priorityMatrix[SYSTEM_SIZE];

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

int getX(int id){
    return id%DIM_X;
}

int getY(int id){
    return (int)(id/DIM_Y);
}

unsigned int API_GetTaskSlotFromTile(unsigned int id, unsigned int app){
    //printf("Testing %d\n", id);
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
        allmodels(TempTraceEnd[structures], power_trace[structures], 1.0, 1.0, structures);
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
    fss = fopen("SystemShot.tsv", "w");
    int id = 0;
    for(int j=0;j<DIM_Y;j++){
        for(int i=0;i<DIM_X;i++){
            if( many_core[j][i].type != -1 ) sprintf(tipo, "%d", many_core[j][i].type);
            else sprintf(tipo, "-");
            fprintf(fss,"(%2d,%2d){%s}\t",j, i, tipo);
            id++;
        }
        fprintf(fss, "\n");
    }
    fclose(fss);
}

void printHeaders(){
    FILE *fl,*fpower,*fp;
    fl = fopen("data/PIDTM_FITlog.tsv", "w");
    fp = fopen("data/PIDTM_SystemTemperature.tsv", "w");
    fpower = fopen("data/PIDTM_SystemPower.tsv", "w");
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

void PIDTM_allocation(int task_to_allocate){
    int k = 0, slot;
    for(int i = 0; i < SYSTEM_SIZE; i++){
        slot = API_GetTaskSlotFromTile(priorityMatrix[i], task_to_allocate);
        //priorityPointer = (priorityPointer + 1) % SYSTEM_SIZE;
        if (slot != -1){
            printf(" - Task %d allocated at addr: %dx%d", task_to_allocate, getY(priorityMatrix[i]), getX(priorityMatrix[i]));
            return;
        }
    }
    printf(" - Retornando sem sucesso!");
    return;
}

void GeneratePatternMatrix(){
    int i, aux;
    aux = 0;
    for(i=0; i<(DIM_X*DIM_Y); i++){
        priorityMatrix[i] = aux;
        aux = aux + 2;
        if(aux >= SYSTEM_SIZE){
            aux = 1;
        }
    }
    return;
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




void PIDTM_UpdateScore(){
    int i;
    for (i = 0; i < SYSTEM_SIZE; i++) {
        if ( measuredWindows >= INT_WINDOW ){
            pidStatus.integral[i] = pidStatus.integral[i] - pidStatus.integral_prev[measuredWindows % INT_WINDOW][i];
        }

        pidStatus.integral_prev[measuredWindows % INT_WINDOW][i] = many_core[getY(i)][getX(i)].temp;

        pidStatus.derivative[i] = many_core[getY(i)][getX(i)].temp - pidStatus.Temperature_prev[i];
        pidStatus.integral[i] = pidStatus.integral[i] + many_core[getY(i)][getX(i)].temp;
        pidStatus.control_signal[i] = (KP * many_core[getY(i)][getX(i)].temp) + (KI * pidStatus.integral[i] / INT_WINDOW) + (KD * pidStatus.derivative[i]);
        pidStatus.Temperature_prev[i] = many_core[getY(i)][getX(i)].temp;
    }
    return;
}

void PIDTM_UpdatePriorityTable(){
    int i;
    int xy;
    int index;
    int coolest = -1;
    //int addr;
    unsigned int score[SYSTEM_SIZE];

    for(i = 0; i < SYSTEM_SIZE; i++){
        score[i] = pidStatus.control_signal[i];
        //printf("score %d = %d\n", i, score[i]);
    }

    for ( i = SYSTEM_SIZE-1; i >= 0; i--) {
        // reset the coolest
        coolest = 0;

        for ( xy = 0; xy < SYSTEM_SIZE; xy++ ) {
            if (score[xy] < score[coolest] && score[xy] != -1)
                coolest = xy;
        }

        //addr = ((coolest % DIM_X) << 8) | (coolest / DIM_X);

        priorityMatrix[i] = coolest;
        //printf("priority %d = %d\n", i, coolest);
        score[coolest] = -1;
    }
}

int main(int argc, char *argv[]){

    srand(time(0));

    // leitura dos arquivos binv e cexp do TEA
    load_matrices();

    if(powerlog == NULL){
        powerlog = fopen("data/PIDTM_SystemPower.tsv", "a");
    }
    if(fp == NULL){
        fp = fopen("data/PIDTM_SystemTemperature.tsv", "a");
    }  
    if(fitlog == NULL){
        fitlog = fopen("data/PIDTM_FITlog.tsv", "a");
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
            many_core[j][i].temp = 273+40;
        }
    }

    printHeaders();

    int cont=0, allocate_task=-1;

    /* Initialize PATTERN matrix */
    GeneratePatternMatrix();

    while(1){
        int barra_ene = 0;    
        printf("\rTime: %.3fs - SysOc: %d",((float)cont/1000), getOccupation());
        cont++;
        measuredWindows = cont;

        // updates the temperature and FIT
        calcula_temp();
        calcula_fit();   

        PIDTM_UpdateScore();
        PIDTM_UpdatePriorityTable();

        // run until 1 sec of simulation
        if(cont == 60000){
            break; 
        } else if( cont % 1000 == 0 ){
            manyCorePrint();
        }
        // if the time is over 20 ms start to evaluate the system
        else if(cont>20){
            // checks if the system is running at target occupation
            if( getOccupation() < int(20*sin(cont/1000) + TARGET_OCCUPATION)  ){
                allocate_task = getNextTask();
                
                if(allocate_task != -1){
                    PIDTM_allocation(allocate_task); barra_ene = 1;
                }
            }
        }

        // write the time into the log files:
        fprintf(fitlog, "%.5f", ((float)cont/1000));
        fprintf(powerlog, "%.5f", ((float)cont/1000));
        fprintf(fp, "%.5f", ((float)cont/1000));

        for(int i=0;i<SYSTEM_SIZE;++i){
            // write info into the log files
            fprintf(powerlog,"\t%f",power_trace[i]);
            fprintf(fp, "\t%.2f", (((float)(TempTraceEnd[i]*100)/100)-273.15));
            many_core[getY(i)][getX(i)].temp = (((float)(TempTraceEnd[i]*100)/100)-273.15);
            fprintf(fitlog,"\t%f",rel_unit[i].ind_inst);
            
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
        fprintf(fitlog,     "\n");
        fprintf(powerlog,   "\n");
        fprintf(fp,         "\n");
        if(barra_ene) printf("\n");
    }
    return 0;
}