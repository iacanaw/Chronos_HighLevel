#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include "reliability.h"
#include "utils.h"

// PIDTM defines
#define KP 10
#define KI 10
#define KD 10
#define INT_WINDOW 10

unsigned int priorityPointer, priorityMatrix[SYSTEM_SIZE];

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

void PIDTM_allocation(int task_to_allocate){
    int k = 0, slot;
    for(int i = SYSTEM_SIZE-1; i >= 0; i--){
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

    // Arquivos
    FILE *ftasks = NULL;
    FILE *powerlog = NULL;
    FILE *fitlog = NULL;
    FILE *fp = NULL;

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
    if(ftasks == NULL){
        ftasks = fopen("tasks", "r");
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
        if(cont == SIM_SEC*1000){
            break; 
        } 
        // else if( cont % 1000 == 0 ){
        //     manyCorePrint();
        // }

        // if the time is over 20 ms start to evaluate the system
        else if(cont>20){
            // checks if the system is running at target occupation
            if( getOccupation() < tagetOccupation(cont))  {
                allocate_task = getNextTask(ftasks);
                
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