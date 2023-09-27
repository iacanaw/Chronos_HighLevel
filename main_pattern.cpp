#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include <math.h>
#include "utils.h"
#include "reliability.h"

#define LOG 1
#define TRAINING 0

/* PATTERN STUFF */
unsigned int priorityPointer, priorityMatrix[SYSTEM_SIZE];

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
    fl = fopen("data/PATTERN_FITlog.tsv", "w");
    fp = fopen("data/PATTERN_SystemTemperature.tsv", "w");
    fpower = fopen("data/PATTERN_SystemPower.tsv", "w");
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

void PATTERN_allocation(int task_to_allocate){
    int k = 0, slot;
    for(int i = 0; i < DIM_X; i++){
        for(int j = 0; j < DIM_Y; j++){
            slot = API_GetTaskSlotFromTile(priorityMatrix[priorityPointer], task_to_allocate);
            priorityPointer = (priorityPointer + 1) % SYSTEM_SIZE;
            if (slot != -1){
                printf(" - Task %d allocated at addr: %dx%d", task_to_allocate, getY(priorityMatrix[priorityPointer]), getX(priorityMatrix[priorityPointer]));
                return;
            }
        }
    }
}

void GeneratePatternMatrix(){
    int i, aux;
    aux = 0;
    priorityPointer = 0;
    for(i=0; i<(DIM_X*DIM_Y); i++){
        priorityMatrix[i] = aux;
        aux = aux + 2;
        if(aux >= SYSTEM_SIZE){
            aux = 1;
        }
    }
    return;
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
        powerlog = fopen("data/PATTERN_SystemPower.tsv", "a");
    }
    if(fp == NULL){
        fp = fopen("data/PATTERN_SystemTemperature.tsv", "a");
    }  
    if(fitlog == NULL){
        fitlog = fopen("data/PATTERN_FITlog.tsv", "a");
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
            many_core[j][i].temp = 0;
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

        // updates the temperature and FIT
        calcula_temp();
        calcula_fit();   

        // run until 1 sec of simulation
        if(cont == SIM_SEC*1000){
            break; 
        } else if( cont % 1000 == 0 ){
            manyCorePrint();
        }
        // STARTING SIMULATION...
        // if the time is over 20 ms start to evaluate the system
        else if(cont>20){
            // checks if the system is running at target occupation
            if( getOccupation() < tagetOccupation(cont) ){
                allocate_task = getNextTask(ftasks);
                
                if(allocate_task != -1){
                    PATTERN_allocation(allocate_task); barra_ene = 1;
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