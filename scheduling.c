#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Structure to represent a process
typedef struct {
    int arrival_time;
    int *bursts;
    int num_bursts;
    int current_burst_index; // Index of current burst being executed
    int remaining_time; // Remaining CPU time for current burst
    int finished; // Flag to indicate if process has finished
    int CPUTurn;
} Process;

void performScheduling(Process processes[], int num_processes) {
}


int main() {
    // Maximum number of bursts in a process
    int max_bursts = 100; // Adjust this value as needed

    Process processes[100]; // Adjust the size as needed
    int num_processes = 0;

    // Read input
    int arrival_time;
    while (scanf("%d", &arrival_time) != EOF) {
        if (arrival_time == -999) {
            // Reached the end of input
            break;
        }
        printf("Arrival time: %d\n", arrival_time);

        // Reset process variables for each new process
        int *bursts = (int *)malloc(max_bursts * sizeof(int));
        int num_bursts = 0;

        // Read CPU and I/O bursts
        int time;
        while (1) {
            scanf("%d", &time);
            if (time == -1)
                break;
            num_bursts%2==0 ? printf("CPU burst: %d\n", time) : printf("IO burst: %d\n", time);
            bursts[num_bursts] = time;
            num_bursts++;
        }

        // Create the process
        Process p;
        p.arrival_time = arrival_time;
        p.bursts = bursts;
        p.num_bursts = num_bursts;
        p.current_burst_index = 0;
        p.remaining_time = bursts[0];
        p.finished = 0;
        p.CPUTurn = 0;
        processes[num_processes++] = p;
    }

    // Perform scheduling
    performScheduling(processes, num_processes);

    // Free allocated memory
    for (int i = 0; i < num_processes; i++) {
        free(processes[i].bursts);
    }

    return 0;
}
