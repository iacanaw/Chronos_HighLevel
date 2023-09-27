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
float scoreTable[N_TASKTYPE][N_STATES] = {  {324237.0, 295195.0, 287508.0, 285065.0, 270329.0, 309018.0, 285011.0, 288375.0, 271410.0, 288543.0, 288562.0, 277399.0, 280248.0, 271141.0, 244175.0, 307707.0, 287938.0, 287855.0, 286293.0, 290539.0, 290118.0, 282242.0, 290476.0, 287227.0, 280388.0, 291726.0, 290776.0, 287595.0, 292417.0, 286941.0, 287158.0, 293754.0, 290471.0, 290603.0, 291297.0, 297806.0, 286080.0, 284385.0, 287026.0, 262177.0, 285094.0, 283815.0, 286411.0, 274827.0, 293329.0, 282637.0, 275086.0, 220773.0, 275904.0, 134824.0, 290571.0, 284396.0, 282803.0, 288085.0, 291406.0, 284692.0, 280116.0, 294256.0, 285579.0, 284585.0, 294808.0, 289358.0, 276039.0, 301347.0, 278731.0, 289297.0, 303952.0, 280151.0, 293210.0, 300779.0, 301228.0, 284493.0, 286633.0, 289318.0, 199160.0, 298560.0, 284523.0, 290816.0, 244011.0, 287182.0, 284074.0, 253665.0, 81348.0, 180100.0, 49583.0, 300043.0, 291946.0, 285042.0, 282395.0, 302828.0, 298075.0, 283148.0, 280432.0, 290751.0, 189154.0, 304181.0, 299137.0, 287614.0, 298929.0, 287866.0, 281446.0, 289083.0, 289969.0, 276282.0, 225178.0, 309787.0, 281892.0, 288519.0, 273049.0, 201086.0, 292647.0, 294183.0, 288645.0, 238246.0, 274847.0, 265152.0, 240851.0, 45292.0, 182583.0, 61385.0, 303448.0, 296936.0, 295824.0, 269495.0, 304753.0, 306670.0, 283490.0, 256636.0, 288283.0, 198552.0, 309857.0, 298174.0, 287637.0, 301621.0, 289301.0, 285747.0, 296619.0, 288980.0, 298442.0, 290174.0, 303134.0, 280033.0, 279387.0, 262844.0, 0.0, 292787.0, 283082.0, 264281.0, 0.0, 290309.0, 266243.0, 0.0, 268405.0, 0.0, 0.0, 292194.0, 284342.0, 273836.0, 0.0, 290463.0, 277013.0, 0.0, 280254.0, 0.0, 0.0, 291494.0, 281330.0, 0.0, 284291.0, 0.0, 0.0, 283134.0, 0.0, 0.0, 0.0 },
 {278829.0, 261023.0, 251259.0, 245183.0, 212682.0, 256089.0, 253153.0, 250076.0, 228388.0, 252593.0, 251969.0, 224473.0, 225137.0, 208107.0, 147845.0, 258614.0, 250987.0, 251394.0, 246205.0, 253499.0, 255025.0, 248296.0, 252916.0, 246972.0, 229878.0, 255417.0, 253319.0, 250179.0, 253706.0, 252240.0, 245782.0, 252888.0, 251520.0, 247301.0, 242704.0, 253532.0, 251240.0, 240984.0, 236350.0, 188989.0, 252198.0, 252545.0, 246206.0, 203424.0, 252451.0, 237267.0, 212015.0, 144231.0, 194863.0, 91867.0, 251979.0, 254122.0, 244599.0, 223726.0, 253586.0, 243969.0, 234368.0, 243070.0, 230489.0, 213859.0, 253116.0, 250953.0, 230981.0, 249020.0, 240086.0, 231951.0, 251975.0, 240379.0, 239614.0, 231185.0, 254180.0, 244063.0, 233614.0, 202547.0, 154358.0, 248274.0, 239758.0, 225462.0, 186750.0, 218252.0, 210627.0, 182772.0, 25911.0, 131626.0, 41425.0, 254040.0, 247583.0, 240112.0, 222542.0, 252160.0, 245278.0, 223373.0, 181202.0, 218621.0, 135367.0, 253523.0, 250303.0, 228586.0, 235443.0, 234029.0, 200824.0, 202881.0, 225645.0, 203339.0, 142752.0, 265848.0, 253778.0, 243193.0, 187027.0, 104208.0, 255366.0, 244926.0, 226106.0, 134790.0, 199966.0, 177989.0, 138253.0, 19806.0, 103452.0, 22809.0, 259866.0, 254578.0, 249177.0, 193563.0, 257763.0, 249645.0, 222872.0, 173118.0, 213252.0, 135432.0, 259635.0, 253805.0, 226881.0, 245276.0, 236783.0, 207964.0, 236287.0, 239486.0, 219184.0, 188805.0, 265308.0, 243793.0, 241869.0, 204267.0, 0.0, 251356.0, 241138.0, 217300.0, 0.0, 251586.0, 211963.0, 0.0, 200664.0, 0.0, 0.0, 257622.0, 248838.0, 230338.0, 0.0, 253714.0, 236780.0, 0.0, 226812.0, 0.0, 0.0, 250000.0, 235190.0, 0.0, 223017.0, 0.0, 0.0, 228156.0, 0.0, 0.0, 0.0 },
 {242795.0, 218014.0, 216632.0, 208812.0, 168472.0, 220349.0, 215443.0, 216524.0, 192378.0, 214925.0, 213691.0, 204284.0, 188210.0, 160599.0, 136734.0, 217014.0, 215317.0, 215752.0, 210058.0, 217115.0, 216057.0, 214811.0, 216466.0, 214595.0, 179255.0, 217482.0, 216524.0, 213841.0, 216057.0, 213471.0, 206791.0, 216213.0, 202233.0, 213765.0, 185093.0, 217285.0, 217352.0, 214169.0, 200413.0, 156372.0, 216099.0, 211536.0, 216143.0, 159347.0, 216089.0, 187784.0, 165176.0, 136988.0, 163755.0, 89479.0, 216774.0, 216832.0, 216019.0, 176223.0, 217335.0, 213926.0, 191237.0, 192053.0, 187812.0, 179330.0, 216559.0, 216260.0, 192724.0, 213262.0, 202509.0, 193105.0, 207457.0, 190153.0, 195624.0, 192838.0, 218073.0, 216773.0, 214885.0, 178707.0, 99951.0, 216301.0, 216858.0, 192287.0, 131972.0, 200583.0, 182483.0, 141749.0, 48225.0, 102565.0, 24580.0, 216831.0, 216839.0, 205395.0, 149430.0, 217875.0, 213227.0, 171909.0, 189211.0, 173515.0, 130399.0, 216554.0, 205563.0, 167976.0, 202542.0, 175272.0, 171287.0, 201052.0, 177994.0, 172582.0, 134220.0, 221974.0, 214870.0, 212318.0, 170061.0, 75311.0, 216326.0, 216241.0, 188799.0, 101479.0, 199009.0, 180607.0, 111236.0, 35833.0, 90701.0, 33547.0, 218003.0, 217593.0, 206303.0, 132258.0, 219306.0, 215789.0, 162322.0, 177035.0, 175063.0, 116143.0, 217631.0, 213396.0, 161145.0, 213540.0, 181989.0, 176150.0, 212284.0, 182059.0, 193570.0, 168772.0, 219470.0, 214105.0, 208175.0, 141494.0, 0.0, 214973.0, 213757.0, 147580.0, 0.0, 211531.0, 145931.0, 0.0, 144495.0, 0.0, 0.0, 218731.0, 215337.0, 161696.0, 0.0, 215272.0, 158016.0, 0.0, 155082.0, 0.0, 0.0, 216623.0, 166878.0, 0.0, 166275.0, 0.0, 0.0, 157559.0, 0.0, 0.0, 0.0 } };

void FLEA535_init(){
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
    fl = fopen("data/FLEA535_FITlog.tsv", "w");
    fp = fopen("data/FLEA535_SystemTemperature.tsv", "w");
    fpower = fopen("data/FLEA535_SystemPower.tsv", "w");
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

void FLEA535_allocation(int task_to_allocate){
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


void FLEA535_training_allocation(int task_to_allocate){
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

void FLEA535_training(int time){
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
        powerlog = fopen("data/FLEA535_SystemPower.tsv", "a");
    }
    if(fp == NULL){
        fp = fopen("data/FLEA535_SystemTemperature.tsv", "a");
    }  
    if(fitlog == NULL){
        fitlog = fopen("data/FLEA535_FITlog.tsv", "a");
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
    FLEA535_init();
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
                    FLEA535_training_allocation(allocate_task); barra_ene = 1;
#else
                    FLEA535_allocation(allocate_task); barra_ene = 1;
#endif
                }
            }
#if TRAINING
            if(cont > 1000) FLEA535_training(cont);
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