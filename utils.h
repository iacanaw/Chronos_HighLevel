#ifndef __UTILS__
#define __UTILS__
#include "reliability.h"

#define DIM_X 20
#define DIM_Y 20
#define N_TASKTYPE  3
#define SYSTEM_SIZE (DIM_X * DIM_Y)  
#define THERMAL_NODES (SYSTEM_SIZE*4)+12  // 4 thermal nodes for each PE plus 12 from the environment
#define TARGET_OCCUPATION 90
#define NUM_TASKS 38
#define SIM_SEC 60

// Estrutura de tasks e do tile
struct Tasks{
    int id,type;
    float power;
    int totalTime, taskSlot, fit, temp, current_time;
};

extern struct floorplan_structure floorplan; 
extern struct UnitRel rel_unit[TOTAL_STRUCTURES];

// Tarefas
extern Tasks tasks[NUM_TASKS];

// Manycore - abstrato
extern Tasks many_core[DIM_Y][DIM_X];

extern double Binv[THERMAL_NODES][SYSTEM_SIZE];
extern double Cexp[THERMAL_NODES][THERMAL_NODES];

extern int power[DIM_Y][DIM_X];
extern double power_trace[SYSTEM_SIZE];
extern double t_steady[THERMAL_NODES];

extern double TempTraceEnd[THERMAL_NODES];
extern double Tsteady[THERMAL_NODES];
extern double Tdifference[THERMAL_NODES];
extern int SystemFIT[DIM_X*DIM_Y];


int getX(int id);
int getY(int id);
int getSouth(int x, int y);
int getNorth(int x, int y);
int getEast(int x, int y);
int getWest(int x, int y);
int getNorthEast(int x, int y);
int getNorthWest(int x, int y);
int getSouthWest(int x, int y);
int getSouthEast(int x, int y);
void swap(int* a, int* b);
int partition (int arr[], int arr2[], int low, int high);
void quickSort(int arr[], int arr2[], int low, int high);
void randPositions(int arr[], int arr2[], int low, int high);
unsigned int API_GetTaskSlotFromTile(unsigned int id, unsigned int app);
unsigned int API_GetTaskSlotFromTile_Mig(unsigned int id, unsigned int app, unsigned int currentTime);
void load_matrices();
void calcula_temp();
void calcula_fit();
int readline(FILE *f, char *buffer, size_t len);
int getNextTask(FILE *ftasks);
int getOccupation();
int tagetOccupation(int cont);

#endif
