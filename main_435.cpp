#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdc++.h>
#include <math.h>
#include "reliability.h"
#include "utils.h"

#define N_STATES 35*4
#define LOG 1

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

float scoreTable[N_TASKTYPE][N_STATES] = {  {234054.0, 169075.0, 165491.0, 150939.0, 142688.0, 180507.0, 175902.0, 172621.0, 151455.0, 183362.0, 163584.0, 154791.0, 149347.0, 142442.0, 146402.0, 205068.0, 175459.0, 169541.0, 176364.0, 182414.0, 171622.0, 177961.0, 174145.0, 170265.0, 166404.0, 186870.0, 177599.0, 179581.0, 182486.0, 177822.0, 169877.0, 181001.0, 180261.0, 177427.0, 183110.0, 180380.0, 170873.0, 163533.0, 176892.0, 167962.0, 178236.0, 164693.0, 183590.0, 172625.0, 180100.0, 176925.0, 166136.0, 133040.0, 161680.0, 74209.0, 205778.0, 174473.0, 185656.0, 188706.0, 187789.0, 177024.0, 178558.0, 179653.0, 177685.0, 164927.0, 197135.0, 185319.0, 181619.0, 196059.0, 186754.0, 177428.0, 198454.0, 192447.0, 191710.0, 188412.0, 203740.0, 162664.0, 171858.0, 151955.0, 128122.0, 197925.0, 178871.0, 175936.0, 149940.0, 181419.0, 166900.0, 144081.0, 31181.0, 100941.0, 33433.0, 205192.0, 173416.0, 178901.0, 169456.0, 191988.0, 185648.0, 171779.0, 174233.0, 163547.0, 110856.0, 204406.0, 183294.0, 175928.0, 192322.0, 172939.0, 142190.0, 184916.0, 178096.0, 158803.0, 121940.0, 210734.0, 166776.0, 167116.0, 91177.0, 104846.0, 199176.0, 186043.0, 162861.0, 129043.0, 187819.0, 137659.0, 123794.0, 15034.0, 88757.0, 34384.0, 209380.0, 169394.0, 175968.0, 177361.0, 201564.0, 191783.0, 173730.0, 170318.0, 166745.0, 104598.0, 197739.0, 209877.0, 187625.0, 196276.0, 178377.0, 160699.0, 194886.0, 190803.0, 178455.0, 170480.0 },
 {153688.0, 135063.0, 119180.0, 107046.0, 59492.0, 128866.0, 124145.0, 124573.0, 71387.0, 128269.0, 122573.0, 70378.0, 59156.0, 62671.0, 50879.0, 139544.0, 128056.0, 124822.0, 128025.0, 128140.0, 127233.0, 124344.0, 112719.0, 126718.0, 86013.0, 130982.0, 128422.0, 125872.0, 128441.0, 127604.0, 109464.0, 125985.0, 128313.0, 118035.0, 107788.0, 153179.0, 120991.0, 111014.0, 87911.0, 57760.0, 124292.0, 113007.0, 116088.0, 76268.0, 109018.0, 97642.0, 84494.0, 56192.0, 68603.0, 29170.0, 134796.0, 124313.0, 121030.0, 100720.0, 126698.0, 125638.0, 97137.0, 123418.0, 106657.0, 89282.0, 131007.0, 126159.0, 104418.0, 131632.0, 112796.0, 95707.0, 125052.0, 120229.0, 112635.0, 92291.0, 153651.0, 114716.0, 100603.0, 66115.0, 56142.0, 121422.0, 114750.0, 98416.0, 71536.0, 115976.0, 92792.0, 65629.0, 6698.0, 56048.0, 13258.0, 137059.0, 123915.0, 113274.0, 82542.0, 123661.0, 114964.0, 93585.0, 93442.0, 95016.0, 47152.0, 130508.0, 119636.0, 99099.0, 118586.0, 103195.0, 76580.0, 107979.0, 100683.0, 92853.0, 63737.0, 151080.0, 123583.0, 113787.0, 49564.0, 52037.0, 138069.0, 117735.0, 95305.0, 58577.0, 124115.0, 70548.0, 55576.0, 8191.0, 36065.0, 16583.0, 149335.0, 127411.0, 113297.0, 89674.0, 130556.0, 121459.0, 91184.0, 92743.0, 92532.0, 49450.0, 147698.0, 127095.0, 94663.0, 119119.0, 116599.0, 66236.0, 108405.0, 100965.0, 88252.0, 65686.0 },
 {96492.0, 59646.0, 59289.0, 56181.0, -39708.0, 64740.0, 59002.0, 59907.0, -34653.0, 58062.0, 57416.0, -32314.0, -2251.0, -49155.0, -43394.0, 68946.0, 64086.0, 58953.0, 28989.0, 59835.0, 60141.0, 36943.0, 57827.0, 9721.0, -31575.0, 63934.0, 59546.0, 44143.0, 58441.0, 30638.0, -4476.0, 58734.0, 23273.0, -5979.0, -7638.0, 88745.0, 59937.0, 48661.0, 16890.0, -38043.0, 60452.0, 58536.0, 25024.0, -12859.0, 46979.0, 10590.0, -9464.0, 30057.0, 4726.0, 8672.0, 63353.0, 58638.0, 59904.0, 5508.0, 59665.0, 56560.0, 11317.0, 33393.0, 17952.0, 4550.0, 59215.0, 60806.0, 34445.0, 50704.0, 33801.0, 20437.0, 50220.0, 30700.0, 25695.0, 25805.0, 73063.0, 58622.0, 33485.0, 11456.0, -33863.0, 62922.0, 42162.0, 10321.0, -13848.0, 39798.0, 23593.0, -13462.0, 10043.0, -4608.0, 6533.0, 61703.0, 60532.0, 29305.0, -11844.0, 59307.0, 31957.0, -3510.0, 42364.0, -12333.0, 1071.0, 61286.0, 22399.0, 2916.0, 30477.0, 12380.0, 17493.0, 33735.0, 377.0, 31099.0, 9288.0, 73820.0, 60917.0, 24594.0, 11072.0, -27236.0, 63596.0, 43125.0, 32933.0, -18020.0, 42470.0, 28116.0, 8055.0, 15329.0, 17605.0, 3351.0, 72436.0, 65673.0, 46438.0, 7016.0, 63432.0, 48833.0, 16415.0, 48724.0, 19879.0, 20987.0, 62902.0, 61940.0, 21263.0, 59444.0, 39518.0, 37050.0, 42761.0, 40899.0, 47477.0, 46711.0 } };


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
    int state_x, state_y, z, state, a;
    int dx, dy, dz;
    int state_dx, state_dy, state_d;

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
    
    x = immediate[0];
    y = immediate[1];
    z = immediate[2];

    dx = diagonal[0];
    dy = diagonal[1];
    dz = diagonal[2];

    state_x = (int)(x ? ((x*x*x - 18*x*x + 107*x) / 6) : 0);
    state_y = (int)(y ? ((11*y - y*y - 2*x*y) / 2) : 0);
    state = state_x + state_y + z;

    if(dz >= 2)
        state = state+35;
    else if(dy >= 2)
        state = state+35*2;
    else if(dx+dy >= 3)
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
    fst = fopen("ScoreTable435.tsv", "w");
    fst2 = fopen("ScoreTable435_vector.tsv", "w");
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
    fss = fopen("SystemShot435.tsv", "w");
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
    freward = fopen("Rewards435.log", "a");
    
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
    //if(time % 100 == 0) API_PrintScoreTable(scoreTable);
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

        // run until 1 sec of simulation
        if(cont == 60000){
            break; 
        } else if( cont % 1000 == 0 ){
            manyCorePrint();
        }
        // STARTING SIMULATION...
        // if the time is over 20 ms start to evaluate the system
        else if(cont>20){
            
            // checks if the system is running at target occupation
            if( getOccupation() < int(20*sin(cont/1000) + TARGET_OCCUPATION)  ){
                allocate_task = getNextTask();
                
                if(allocate_task != -1){
                    //FLEA_training_allocation(allocate_task); barra_ene = 1;
                    FLEA_allocation(allocate_task); barra_ene = 1;
                }
            }
            //if(cont > 1000) FLEA_training(cont);
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