#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include <math.h>
#include "utils.h"
#include "reliability.h"

#define N_STATES 35*5
#define LOG 1
#define TRAINING 0

// Training stuff
unsigned int tableUpdates[N_TASKTYPE][N_STATES];
int state_last[DIM_X*DIM_Y], starting_fit[DIM_X*DIM_Y], state_stability[DIM_X*DIM_Y];

// The score table
float scoreTable[N_TASKTYPE][N_STATES] = {  {358110.0, 311819.0, 326506.0, 287510.0, 267868.0, 336125.0, 334612.0, 309804.0, 281565.0, 333904.0, 316907.0, 291981.0, 332334.0, 286459.0, 289780.0, 342553.0, 326100.0, 322869.0, 294792.0, 348851.0, 328519.0, 297583.0, 328653.0, 300688.0, 302828.0, 341748.0, 328758.0, 306349.0, 325399.0, 302511.0, 314524.0, 338808.0, 314300.0, 321710.0, 333099.0 },
                                            {281342.0, 273720.0, 236944.0, 233849.0, 193761.0, 282903.0, 267191.0, 235700.0, 210713.0, 268933.0, 252355.0, 220726.0, 257638.0, 221475.0, 235138.0, 289472.0, 263675.0, 239016.0, 213860.0, 268871.0, 254036.0, 229620.0, 267621.0, 230426.0, 235955.0, 267128.0, 264112.0, 234761.0, 268650.0, 236529.0, 238415.0, 274209.0, 247714.0, 251370.0, 260251.0 },
                                            {210297.0, 193433.0, 191665.0, 158729.0, 125676.0, 207265.0, 200705.0, 163408.0, 131866.0, 201219.0, 173236.0, 153222.0, 176975.0, 145328.0, 150652.0, 206857.0, 191963.0, 172304.0, 143225.0, 203435.0, 184750.0, 153688.0, 183816.0, 152199.0, 154680.0, 197556.0, 187876.0, 148384.0, 200729.0, 157836.0, 156609.0, 204668.0, 163326.0, 168297.0, 166374.0 } };

void FLEA35_init(){
    for(int i = 0; i < SYSTEM_SIZE; i++){
        state_last[i] = -1;
        starting_fit[i] = -1;
        state_stability[i] = 0;
    }
}

//The place of x will be informed by sucessives sums and the y by num(addr%DIM_X or DIM_Y)
unsigned int API_getPEState(unsigned int id){
    unsigned int x = getX(id);
    unsigned int y = getY(id);
    int state_0, state_1, state, a;
    int i0, i1, i2;

    unsigned int immediate[3];
    
    for(a = 0; a < 3; a++){
        immediate[a] = 0;
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
    
    i0 = immediate[0];
    i1 = immediate[1];
    i2 = immediate[2];

    state_0 = (int)(i0 ? ((i0*i0*i0 - 18*i0*i0 + 107*i0) / 6) : 0);
    state_1 = (int)(i1 ? ((11*i1 - i1*i1 - 2*i0*i1) / 2) : 0);
    state = state_0 + state_1 + i2;

    if(state >= N_STATES){
        printf("ERRO CALCULANDO ESTADO: %d", state);
        while(1){}
    }
    return state;
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
    fl = fopen("data/FLEA35_FITlog.tsv", "w");
    fp = fopen("data/FLEA35_SystemTemperature.tsv", "w");
    fpower = fopen("data/FLEA35_SystemPower.tsv", "w");
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

void FLEA35_allocation(int task_to_allocate){
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


void FLEA35_training_allocation(int task_to_allocate){
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

void FLEA35_training(int time){
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
                reward = 50.0+(delta*delta);
                //reward = Q_rsqrt(reward);
                reward = 1/sqrt(reward);
                reward = ((-200.0 * reward * delta)+100.0);
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
#if TRAINING
    if(time % 100 == 0) API_PrintScoreTable(scoreTable);
    printf(" - Updates so far: %ld", getTotalUpdated());
#endif
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
        powerlog = fopen("data/FLEA35_SystemPower.tsv", "a");
    }
    if(fp == NULL){
        fp = fopen("data/FLEA35_SystemTemperature.tsv", "a");
    }  
    if(fitlog == NULL){
        fitlog = fopen("data/FLEA35_FITlog.tsv", "a");
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
    
    /* FLEA INITIALIZATION */
    FLEA35_init();
    for(int i = 0; i < N_TASKTYPE; i++){
        for(int j = 0; j < N_STATES; j++ ){
#if TRAINING
            tableUpdates[i][j] = 0;
#endif
            scoreTable[i][j] = scoreTable[i][j] / 1000.0;
        }
    }
    
    while(1){
        int barra_ene = 0;    
        printf("\rTime: %.3fs - SysOc: %d",((float)cont/1000), getOccupation());
        cont++;

        // updates the temperature and FIT
        calcula_temp();
        calcula_fit();   

        // run until 60 sec of simulation
#if !TRAINING
        if(cont == SIM_SEC*1000){
            break; 
        } 
        // else if( cont % 1000 == 0 ){
        //     manyCorePrint();
        // }
#endif

        // STARTING SIMULATION...
        // if the time is over 20 ms start to evaluate the system
        else if(cont>20){
            
            // checks if the system is running at target occupation
            if( getOccupation() < tagetOccupation(cont))  {
                allocate_task = getNextTask(ftasks);
                
                if(allocate_task != -1){
#if TRAINING
                    FLEA35_training_allocation(allocate_task); barra_ene = 1;
#else
                    FLEA35_allocation(allocate_task); barra_ene = 1;
#endif
                }
            }
#if TRAINING
            if(cont > 1000) FLEA35_training(cont);
#endif
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
