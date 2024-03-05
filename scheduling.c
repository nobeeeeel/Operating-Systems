#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

// Structure to represent a process
typedef struct {
    int turnAroundTime;
    int arrival_time;
    int *bursts;
    int num_bursts;
    int current_burst_index; // Index of current burst being executed
    int remaining_time; // Remaining CPU time for current burst
    bool finished; // Flag to indicate if process has finished
    bool CPUTurn;
    bool inProcess;
} Process;

void averageTurnAroundTime(Process processes[], int numberOfProcesses){
    double sum = 0;
    for(int idx = 0; idx<numberOfProcesses; idx++){
        sum+=processes[idx].turnAroundTime;
    }
    printf("%.0lf\n", sum/numberOfProcesses);
}

int finishedProcessing(Process processes[], int numbersOfProcesses){
    for(int idx = 0; idx < numbersOfProcesses; idx++){
        if(!processes[idx].finished){
            return 0;
        }
    }
    return 1;
}

void nextProcessIndex(Process processes[], int numOfProc, bool CPUFlag, bool IOFlag, int *nextIdx){
    int minArrivalTime = -2;
    for(int idx = 0; idx < numOfProc; idx++){
        if(!processes[idx].inProcess && !processes[idx].finished && processes[idx].arrival_time>=0 && (processes[idx].CPUTurn == CPUFlag || processes[idx].CPUTurn != IOFlag)){
            if(minArrivalTime == -2){
                minArrivalTime = processes[idx].arrival_time;
                *nextIdx = idx;
            }
            if(minArrivalTime > processes[idx].arrival_time){
                minArrivalTime = processes[idx].arrival_time;
                *nextIdx = idx;
            }
        }
    }
    if(minArrivalTime==-2)
        *nextIdx= -2;
}

void performScheduling(Process processes[], int num_processes) {
    int currentTime = 0;
    int nextFreeSpot = 0;
    bool CPUInUse = false;
    bool IOInUse = false;
    int nextIdx = 0;
    int CPUIndex = -1;
    int IOIndex = -1;

    //Performs the scheduling operation from this point, nothing is being run
    //And every thing is initialized before this point
    while(!finishedProcessing(processes, num_processes)){
        //-2 equals no new index
        nextProcessIndex(processes, num_processes, CPUInUse, IOInUse, &nextIdx);

        //check if someone can use CPU
        if(!CPUInUse){ //Checks for nextidx
            if(nextIdx!=-2 && processes[nextIdx].CPUTurn && !processes[nextIdx].finished && !processes[nextIdx].inProcess){
                CPUInUse = true;
                processes[nextIdx].remaining_time = processes[nextIdx].bursts[processes[nextIdx].current_burst_index];
                processes[nextIdx].turnAroundTime+= currentTime -  processes[nextIdx].arrival_time +  processes[nextIdx].remaining_time;
                processes[nextIdx].arrival_time = currentTime + processes[nextIdx].bursts[processes[nextIdx].current_burst_index];
                processes[nextIdx].current_burst_index++;
                processes[nextIdx].CPUTurn = false;
                processes[nextIdx].inProcess = true;
                CPUIndex = nextIdx;
            } else { //Check if anyone else can use it
                int minimum_time = -1;
                for(int i = 0; i<num_processes; i++){
                    if(minimum_time == -1 && processes[i].CPUTurn && !processes[i].finished && !processes[i].inProcess){
                        CPUIndex = i;
                        minimum_time = processes[i].arrival_time;
                    }
                    if(processes[i].CPUTurn && processes[i].arrival_time < minimum_time && !processes[i].finished && !processes[i].inProcess){
                        CPUIndex = i;
                        minimum_time = processes[i].arrival_time;
                    }
                }

                if(minimum_time>0){ //If minimum_time > 0 means someone got chosen
                    CPUInUse = true;
                    processes[CPUIndex].remaining_time = processes[CPUIndex].bursts[processes[CPUIndex].current_burst_index];
                    processes[CPUIndex].turnAroundTime+= currentTime -  processes[CPUIndex].arrival_time +  processes[CPUIndex].remaining_time;
                    processes[CPUIndex].arrival_time = currentTime + processes[CPUIndex].bursts[processes[CPUIndex].current_burst_index];
                    processes[CPUIndex].current_burst_index++;
                    processes[CPUIndex].CPUTurn = true;
                    processes[CPUIndex].inProcess = true;
                }
            }
        }
        if(!IOInUse){ //Same as above for IO
            if(nextIdx!=-2 &&!processes[nextIdx].CPUTurn && !processes[nextIdx].finished && !processes[nextIdx].inProcess){
                IOInUse = true;
                processes[nextIdx].remaining_time = processes[nextIdx].bursts[processes[nextIdx].current_burst_index];
                processes[nextIdx].turnAroundTime+= currentTime -  processes[nextIdx].arrival_time +  processes[nextIdx].remaining_time;
                processes[nextIdx].arrival_time = currentTime + processes[nextIdx].bursts[processes[nextIdx].current_burst_index];
                processes[nextIdx].current_burst_index++;
                processes[nextIdx].CPUTurn = true;
                processes[nextIdx].inProcess = true;
                IOIndex = nextIdx;
            } else { //Check if anyone else can use it
                int minimum_time = -1;
                for(int i = 0; i<num_processes; i++){
                    if(minimum_time == -1 && !processes[i].CPUTurn && !processes[i].finished && !processes[i].inProcess){
                        IOIndex = i;
                        minimum_time = processes[i].arrival_time;
                    }
                    if(!processes[i].CPUTurn && processes[i].arrival_time < minimum_time && !processes[i].finished && !processes[i].inProcess){
                        IOIndex = i;
                        minimum_time = processes[i].arrival_time;
                    }
                }
                if(minimum_time>0){
                    IOInUse = true;
                    processes[IOIndex].remaining_time = processes[IOIndex].bursts[processes[IOIndex].current_burst_index];
                    processes[IOIndex].turnAroundTime+= currentTime -  processes[IOIndex].arrival_time +  processes[IOIndex].remaining_time;
                    processes[IOIndex].arrival_time = currentTime + processes[IOIndex].bursts[processes[IOIndex].current_burst_index];
                    processes[IOIndex].current_burst_index++;
                    processes[IOIndex].CPUTurn = true;
                    processes[IOIndex].inProcess = true;
                }
            }
        }

        if(CPUInUse && IOInUse){ //Solves if both are in use
            if(processes[IOIndex].remaining_time > processes[CPUIndex].remaining_time){
                processes[IOIndex].remaining_time-=processes[CPUIndex].remaining_time;
                nextFreeSpot = processes[CPUIndex].remaining_time;
                processes[CPUIndex].remaining_time = 0;
                processes[CPUIndex].inProcess = false;
                CPUInUse = false;
            } else {
                processes[CPUIndex].remaining_time-=processes[IOIndex].remaining_time;
                nextFreeSpot = processes[IOIndex].remaining_time;
                processes[IOIndex].remaining_time = 0;
                processes[IOIndex].inProcess = false;
                IOInUse = false;
            }
        } else if(CPUInUse){ //solves if cpu is in use
            nextFreeSpot = processes[CPUIndex].remaining_time;
            processes[CPUIndex].remaining_time = 0;
            processes[CPUIndex].inProcess = false;
            CPUInUse = false;
        } else { //Solves if IO is in use
            nextFreeSpot = processes[IOIndex].remaining_time;
            processes[IOIndex].remaining_time = 0;
            processes[IOIndex].inProcess = false;
            IOInUse = false;
        }

        //if burst index equals number of bursts, mark process as finished
        if(processes[IOIndex].current_burst_index >= processes[IOIndex].num_bursts){
            processes[IOIndex].finished = true;
        }

        if(processes[CPUIndex].current_burst_index >= processes[CPUIndex].num_bursts){
            processes[CPUIndex].finished = true;
        }


        currentTime += nextFreeSpot;
    }
}

void freeMemory(Process processes[], int num_processes){
    for (int i = 0; i < num_processes; i++) {
        //free(processes[i].bursts);
    }
    //free(processes);
}


int main() {
    Process *processes = NULL;
    int num_processes = 0;

    // Read input
    int arrival_time;
    while (scanf("%d", &arrival_time) != EOF) {
        Process p;
        p.arrival_time = arrival_time;

        // Reset process variables for each new process
        int *bursts = NULL;
        int num_bursts = 0;

        // Read bursts
        int time;
        while (scanf("%d", &time), time != -1) {
            bursts = (int *)realloc(bursts, (num_bursts + 1) * sizeof(int));
            if (bursts == NULL) {
                fprintf(stderr, "Memory allocation failed.\n");
                return 1;
            }
            bursts[num_bursts] = time;
            num_bursts++;
        }

        // Create the process
        p.turnAroundTime = 0;
        p.bursts = bursts;
        p.num_bursts = num_bursts;
        p.current_burst_index = 0;
        p.remaining_time = 0;
        p.finished = false;
        p.CPUTurn = true;
        p.inProcess = false;

        // Resize the processes array as needed
        processes = (Process *)realloc(processes, (num_processes + 1) * sizeof(Process));
        if (processes == NULL) {
            fprintf(stderr, "Memory allocation failed. Exiting...\n");
            return 1;
        }

        processes[num_processes] = p;
        num_processes++;
    }

    // Perform scheduling
    performScheduling(processes, num_processes);
    averageTurnAroundTime(processes, num_processes);

    // Free allocated memory
    freeMemory(processes, num_processes);

    return 0;
}
