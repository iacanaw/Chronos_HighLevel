#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include <math.h>
#include "utils.h"
#include "reliability.h"

#define N_STATES 35*5
#define LOG 0
#define TRAINING 1

// Training stuff
unsigned int tableUpdates[N_TASKTYPE][N_STATES];
int state_last[DIM_X*DIM_Y], state_stability[DIM_X*DIM_Y];
float starting_fit[DIM_X*DIM_Y];

// The score table
float scoreTable[N_TASKTYPE][N_STATES] = {  {322252.0, 287970.0, 288635.0, 285608.0, 263401.0, 292792.0, 289590.0, 288170.0, 279801.0, 292299.0, 286347.0, 274890.0, 285993.0, 269515.0, 272779.0, 300116.0, 292379.0, 295042.0, 279412.0, 295081.0, 292591.0, 281402.0, 286087.0, 285034.0, 283156.0, 296765.0, 296017.0, 287151.0, 296173.0, 288831.0, 292244.0, 296070.0, 291868.0, 296704.0, 296881.0, 292702.0, 285508.0, 285765.0, 284135.0, 268599.0, 296172.0, 284608.0, 281901.0, 273260.0, 295257.0, 282654.0, 278148.0, 270772.0, 283881.0, 213243.0, 295472.0, 295144.0, 280866.0, 278888.0, 295305.0, 284237.0, 279199.0, 294432.0, 279993.0, 292917.0, 305230.0, 286862.0, 272268.0, 290457.0, 277769.0, 295375.0, 300207.0, 284870.0, 290797.0, 296175.0, 294093.0, 290599.0, 302916.0, 285645.0, 251885.0, 297855.0, 298107.0, 289498.0, 270951.0, 298715.0, 294172.0, 277418.0, 102622.0, 236406.0, 80429.0, 298673.0, 295783.0, 286379.0, 281870.0, 302070.0, 292254.0, 284818.0, 297442.0, 285425.0, 261118.0, 302414.0, 292240.0, 287606.0, 308857.0, 287324.0, 295047.0, 306519.0, 294581.0, 299957.0, 295595.0, 304439.0, 285145.0, 296363.0, 295948.0, 239087.0, 295115.0, 296422.0, 305352.0, 266815.0, 300527.0, 297592.0, 273688.0, 79315.0, 241686.0, 91208.0, 311835.0, 293923.0, 301943.0, 282634.0, 310255.0, 315933.0, 283965.0, 299149.0, 297737.0, 283945.0, 308881.0, 300833.0, 287222.0, 305120.0, 292862.0, 293665.0, 303697.0, 293638.0, 297572.0, 301001.0, 298303.0, 279320.0, 271028.0, 260458.0, 0.0, 292143.0, 285040.0, 260003.0, 0.0, 290458.0, 266749.0, 0.0, 265302.0, 0.0, 0.0, 302475.0, 281125.0, 277260.0, 0.0, 293553.0, 275016.0, 0.0, 278359.0, 0.0, 0.0, 289600.0, 281257.0, 0.0, 283270.0, 0.0, 0.0, 285173.0, 0.0, 0.0, 0.0 },
 {275663.0, 261577.0, 261440.0, 256430.0, 214461.0, 263458.0, 261713.0, 254778.0, 236082.0, 261410.0, 252164.0, 245960.0, 242599.0, 226824.0, 197813.0, 265627.0, 260089.0, 259972.0, 249450.0, 260759.0, 260448.0, 254897.0, 252002.0, 258603.0, 247461.0, 261935.0, 259853.0, 256218.0, 261811.0, 260413.0, 261568.0, 262483.0, 258157.0, 260568.0, 246901.0, 254967.0, 255733.0, 252653.0, 244174.0, 213029.0, 260040.0, 252844.0, 251664.0, 217396.0, 257375.0, 246377.0, 223772.0, 178093.0, 225110.0, 141539.0, 260905.0, 258529.0, 254550.0, 229543.0, 259024.0, 259777.0, 241095.0, 251509.0, 246915.0, 241263.0, 260774.0, 258448.0, 246933.0, 261648.0, 246404.0, 249114.0, 259117.0, 254358.0, 256749.0, 251718.0, 263007.0, 256919.0, 249051.0, 237194.0, 195839.0, 263104.0, 257353.0, 241686.0, 217225.0, 251861.0, 247317.0, 216912.0, 40075.0, 181847.0, 63209.0, 262715.0, 259362.0, 251843.0, 231290.0, 261755.0, 254919.0, 241972.0, 234185.0, 243409.0, 206562.0, 264874.0, 260226.0, 242497.0, 261033.0, 246262.0, 245188.0, 250897.0, 256412.0, 251341.0, 215734.0, 268076.0, 271510.0, 261357.0, 227462.0, 155077.0, 265182.0, 258552.0, 256937.0, 197125.0, 251946.0, 232329.0, 210332.0, 33426.0, 168486.0, 50106.0, 271477.0, 264678.0, 253122.0, 226494.0, 269344.0, 265430.0, 239691.0, 237671.0, 249272.0, 207399.0, 273184.0, 263761.0, 241270.0, 267831.0, 251290.0, 247771.0, 264966.0, 254683.0, 258909.0, 258149.0, 270180.0, 248783.0, 239196.0, 216118.0, 0.0, 259324.0, 249660.0, 217946.0, 0.0, 258386.0, 224207.0, 0.0, 210907.0, 0.0, 0.0, 261518.0, 255286.0, 240042.0, 0.0, 259837.0, 242205.0, 0.0, 229742.0, 0.0, 0.0, 261818.0, 239909.0, 0.0, 234103.0, 0.0, 0.0, 234439.0, 0.0, 0.0, 0.0 },
 {232674.0, 214384.0, 216160.0, 209346.0, 145656.0, 217763.0, 214993.0, 206756.0, 172906.0, 214152.0, 209847.0, 179253.0, 190672.0, 172529.0, 153859.0, 216983.0, 214106.0, 213816.0, 198323.0, 214076.0, 213800.0, 203305.0, 213250.0, 205994.0, 173100.0, 214512.0, 213459.0, 210358.0, 214466.0, 207377.0, 201729.0, 208407.0, 206154.0, 205950.0, 185289.0, 213161.0, 210907.0, 208729.0, 188024.0, 145310.0, 211525.0, 203965.0, 204073.0, 152315.0, 201238.0, 189518.0, 156101.0, 176036.0, 160851.0, 145592.0, 213405.0, 213630.0, 204350.0, 160661.0, 208561.0, 203617.0, 190348.0, 191447.0, 179546.0, 173331.0, 214412.0, 205011.0, 185170.0, 202123.0, 200009.0, 184666.0, 201758.0, 194377.0, 186434.0, 191581.0, 216303.0, 213881.0, 207886.0, 172117.0, 124248.0, 214772.0, 210645.0, 181130.0, 143401.0, 196237.0, 189623.0, 152096.0, 68253.0, 138083.0, 56243.0, 220264.0, 212792.0, 186029.0, 147430.0, 212835.0, 193967.0, 156789.0, 188484.0, 162763.0, 162286.0, 209711.0, 188531.0, 161895.0, 200791.0, 167560.0, 182429.0, 195343.0, 179509.0, 179533.0, 179907.0, 224799.0, 220156.0, 206125.0, 182197.0, 106470.0, 218484.0, 211340.0, 186721.0, 139266.0, 203658.0, 193429.0, 150097.0, 67387.0, 131233.0, 72341.0, 223584.0, 215854.0, 195708.0, 150272.0, 217453.0, 206248.0, 161915.0, 208062.0, 164841.0, 162018.0, 215413.0, 207983.0, 162157.0, 213817.0, 173568.0, 178535.0, 210611.0, 177172.0, 189630.0, 187804.0, 221062.0, 213271.0, 204976.0, 134595.0, 0.0, 213319.0, 204014.0, 134497.0, 0.0, 197891.0, 142113.0, 0.0, 132186.0, 0.0, 0.0, 214014.0, 212694.0, 153647.0, 0.0, 213525.0, 147459.0, 0.0, 160542.0, 0.0, 0.0, 207906.0, 155615.0, 0.0, 150727.0, 0.0, 0.0, 155896.0, 0.0, 0.0, 0.0 } };

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
    FILE *fst, *fst2, *fupdt;
    fupdt = fopen("ScoreTableUpdate535.tsv", "w");
    fst = fopen("ScoreTable535.tsv", "w");
    fst2 = fopen("ScoreTable535_vector.tsv", "w");
    fprintf(fst2, "float scoreTable[N_TASKTYPE][N_STATES] = { ");
    for(i = 0; i < N_TASKTYPE; i++){
        fprintf(fst2, " {");
        for(j = 0; j < N_STATES; j++){
            fprintf(fst,"%d",(int)(scoreTable[i][j]*1000));
            fprintf(fupdt,"%d",(int)(tableUpdates[i][j]));
            fprintf(fst2,"%.1f",(float)((int)(scoreTable[i][j]*1000)));
            //fprintf(fst,"%d",(int)(scoreTable[i][j]*1000));
            if(j != N_STATES-1){
                fprintf(fst,"\t");
                fprintf(fupdt,"\t");
                fprintf(fst2,", ");
            }
        }
        fprintf(fst2, " }");
        if (i != N_TASKTYPE-1){
            fprintf(fst,"\n");
            fprintf(fupdt,"\n");
            fprintf(fst2,",\n");
        }
    }
    fprintf(fst2," };");
    fclose(fst);
    fclose(fst2);
    fclose(fupdt);
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

void FLEA535_allocation_cluster(int task_to_allocate, int cluster_id, int cluster_side){
    int sorted_id[DIM_X*DIM_Y], sorted_score[DIM_X*DIM_Y];
    int k = 0;
    int x_cluster = getX(cluster_id);
    int y_cluster = getY(cluster_id);
    for(int i = x_cluster; i < x_cluster+cluster_side; i++){
        for(int j = y_cluster; j < y_cluster+cluster_side; j++){
            sorted_id[k] = i + j*DIM_X;
            sorted_score[k] = (int)(scoreTable[tasks[task_to_allocate].type][API_getPEState(k)]*1000);
            k++;
        }
    }
    quickSort(sorted_score, sorted_id, 0, (cluster_side*cluster_side)-1);
    int id, slot = -1;
    for(int i = (cluster_side*cluster_side)-1; i >= 0; i--){
        id = sorted_id[i];
        //printf("Testing id: %d \n", id);
        slot = API_GetTaskSlotFromTile(id, task_to_allocate);
        if (slot != -1) break;
    }
    if (slot != -1)
        printf(" - Task %d allocated at addr: %dx%d", task_to_allocate, getY(id), getX(id));
    else{
        printf(" - Task %d not allocated!!:\n", task_to_allocate); 
        throw 1;
    }
}

void FLEA535_migrate(int id_src){
    int sorted_id[DIM_X*DIM_Y], sorted_score[DIM_X*DIM_Y];
    int k = 0;

    int task_to_allocate = many_core[getY(id_src)][getX(id_src)].id;
    many_core[getY(id_src)][getX(id_src)].current_time = -1;

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
        slot = API_GetTaskSlotFromTile_Mig(id, task_to_allocate, many_core[getY(id)][getX(id)].current_time);
        if (slot != -1) break;
    }
    if (slot != -1){
        printf(" - Migrate: PE %d -> PE %d", id_src, id);
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

    // FILE *freward;
    // freward = fopen("Rewards535.log", "a");
    
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
                //fprintf(freward, "%.4f; %.4f\n", delta, reward);
                 
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
    // fclose(freward);
    // print score table
#if TRAINING
    if(time % 1000 == 0) API_PrintScoreTable(scoreTable);
    printf(" - Updates so far: %ld", getTotalUpdated());
#endif
}

unsigned int getCluster(int n){
    int side = (int)sqrt((2*n))+1;
    int slide = 2;

    int selected = 0;

    unsigned int cluster;

    int occupation = 0;
    float temperature = 0;
    float fit = 0;

    float sel_temp = -1;
    int sel_fit = -1;
    float sel_occ = -1;
    int sel_id;
    //printf("Procurando um cluster para n: %d - 2n: %d - side: %d\n", n, 2*n, side);
    do{
        // printf("Procurando um cluster com side = %d\n", side);
        for(int x = 0; x <= DIM_X-side; x+=slide){
            for(int y = 0; y <= DIM_Y-side; y+=slide){
                temperature = 0.0;
                fit = 0.0;
                occupation = 0;
                // printf("Considerando o cluster que comeca em %dx%d\n", x, y);
                for(int xx = x; xx < x+side; xx++){
                    for(int yy = y; yy < y+side; yy++){
                        temperature += many_core[yy][xx].temp;
                        fit += many_core[yy][xx].fit;
                        occupation += many_core[yy][xx].taskSlot;
                        // printf("Analisando PE %dx%d\n", xx, yy);
                    }
                }
                if(occupation >= 2*n){
                    selected = 1;
                    // pega o primeiro cluster com espaço, ou o mais frio, se empatar na temperature, sorteia um
                    if(sel_temp == -1 || sel_temp > temperature || (sel_temp == temperature && rand()%2 == 0)){
                        sel_temp = temperature;
                        sel_fit = fit;
                        sel_occ = occupation;
                        sel_id = y*DIM_X + x;
                        // printf("Cluster selecionado PE %dx%d - ID %d\n", x, y, sel_id);
                    }
                }
            }
        }
        if(selected) break;
        else side++;
    }while(1);
    return sel_id<<16 | side;
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
            many_core[j][i].temp = 0.0;
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
    
    int cluster_count = 0;
    int cluster_id = 0;
    int cluster_side = 0;

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
        if(cont>20){
            
            // checks if the system is running at target occupation
            if( getOccupation() < tagetOccupation(cont))  {
                allocate_task = getNextTask(ftasks);
                
                if(cluster_count == 0){
                    cluster_count = ((allocate_task+60)%6)+2;
                    cluster_side = getCluster(cluster_count);
                    cluster_id = cluster_side >> 16;
                    cluster_side = cluster_side & 0x0000FFFF;
                    printf(" - Cluster final - side: %d - ID: %d", cluster_side, cluster_id);
                }

                if(allocate_task != -1){
#if TRAINING
                    FLEA535_training_allocation(allocate_task); barra_ene = 1;
#else
                    FLEA535_allocation_cluster(allocate_task, cluster_id, cluster_side); barra_ene = 1;
                    cluster_count--;
#endif
                }
            }
#if TRAINING
            if(cont > 1000) FLEA535_training(cont);
#endif
        }

        // Migrate
        int higherTempID = 0;
        if(cont%20 == 0){
            // find the higher temp PE
            for(int i = 0; i < SYSTEM_SIZE; i++){
                //printf("temp: %d %d\n", i, many_core[getY(i)][getX(i)].temp);
                if(many_core[getY(i)][getX(i)].temp > many_core[getY(higherTempID)][getX(higherTempID)].temp){
                    if(many_core[getY(i)][getX(i)].type != -1){
                        higherTempID = i;
                    }
                }
            }
            //
            FLEA535_migrate(higherTempID);
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
            many_core[getY(i)][getX(i)].temp = (((float)(TempTraceEnd[i]*100)/100)-273.15);
            fprintf(fitlog,"\t%f",rel_unit[i].ind_inst);
#endif  
            if(many_core[getY(i)][getX(i)].type != -1) { 
                // checks if the task has finished
                if(many_core[getY(i)][getX(i)].current_time >= many_core[getY(i)][getX(i)].totalTime || many_core[getY(i)][getX(i)].current_time == -1) {
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