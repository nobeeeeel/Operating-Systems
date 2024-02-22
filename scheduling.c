#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Structure to represent a process
typedef struct {
    int arrival_time;
    int *cpu_bursts;
    int *io_bursts;
    int num_bursts;
    int current_burst_index; // Index of current burst being executed
    int remaining_cpu_time; // Remaining CPU time for current burst
    int remaining_io_time; // Remaining I/O time for current burst
    int finished; // Flag to indicate if process has finished
} Process;

void performScheduling(Process processes[], int num_processes) {
    int current_time = 0; // Current time
    int cpu_process_index = -1; // Index of process currently using CPU
    int io_process_index = -1; // Index of process currently doing I/O
    int io_completion_time = -1; // Time when the current I/O operation completes

    while (1) {
        // Check if all processes have finished
        int all_finished = 1;
        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].finished) {
                all_finished = 0;
                break;
            }
        }
        if (all_finished) {
            break;
        }

        // Check if any new processes arrive
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == current_time && !processes[i].finished) {
                printf("Process with arrival time %d arrived\n", current_time);
                // Queue the new process
                if (cpu_process_index == -1) {
                    cpu_process_index = i; // Assign CPU to the new process if CPU is idle
                } else if (io_process_index == -1) {
                    io_process_index = i; // If CPU is busy, assign to I/O if I/O is idle
                    io_completion_time = current_time + processes[i].remaining_io_time;
                }
            }
        }

        // Check if any I/O operation completes
        if (io_completion_time == current_time && io_process_index != -1) {
            printf("I/O operation completed for process %d\n", io_process_index);
            cpu_process_index = io_process_index; // Move the process from I/O to CPU
            io_process_index = -1;
            io_completion_time = -1;
        }

        // Execute CPU burst for the process using CPU
        if (cpu_process_index != -1) {
            printf("Executing CPU burst for process %d\n", cpu_process_index);
            processes[cpu_process_index].remaining_cpu_time--;
            if (processes[cpu_process_index].remaining_cpu_time == 0) {
                // Process finishes its CPU burst
                printf("Process %d finished its CPU burst\n", cpu_process_index);
                processes[cpu_process_index].current_burst_index++;
                if (processes[cpu_process_index].current_burst_index == processes[cpu_process_index].num_bursts) {
                    // Process has finished all bursts
                    printf("Process %d finished all bursts\n", cpu_process_index);
                    processes[cpu_process_index].finished = 1;
                } 
                // Find the next process to run on CPU (based on priority of arrival time)
                int next_process_index = -1;
                int earliest_arrival_time = INT_MAX;
                for (int i = 0; i < num_processes; i++) {
                    if (!processes[i].finished && processes[i].arrival_time < earliest_arrival_time) {
                        next_process_index = i;
                        earliest_arrival_time = processes[i].arrival_time;
                    }
                }
                if (next_process_index != -1) {
                    printf("Switching to process %d due to higher priority\n", next_process_index);
                    cpu_process_index = next_process_index;
                } else {
                    cpu_process_index = -1; // No more processes to run
                }
            }
        }

        current_time++;
    }
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
        int *cpu_bursts = (int *)malloc(max_bursts * sizeof(int));
        int *io_bursts = (int *)malloc(max_bursts * sizeof(int));
        int num_bursts = 0;

        // Read CPU and I/O bursts
        int cpu_time, io_time;
        while (1) {
            scanf("%d", &cpu_time);
            if (cpu_time == -1)
                break;
            printf("CPU burst: %d\n", cpu_time);
            cpu_bursts[num_bursts] = cpu_time;
            num_bursts++;

            scanf("%d", &io_time);
            if (io_time == -1)
                break;
            printf("IO burst: %d\n", io_time);
            io_bursts[num_bursts - 1] = io_time;
        }

        // Create the process
        Process p;
        p.arrival_time = arrival_time;
        p.cpu_bursts = cpu_bursts;
        p.io_bursts = io_bursts;
        p.num_bursts = num_bursts;
        p.current_burst_index = 0;
        p.remaining_cpu_time = cpu_bursts[0];
        p.remaining_io_time = io_bursts[0];
        p.finished = 0;
        processes[num_processes++] = p;
    }

    // Perform scheduling
    performScheduling(processes, num_processes);

    // Free allocated memory
    for (int i = 0; i < num_processes; i++) {
        free(processes[i].cpu_bursts);
        free(processes[i].io_bursts);
    }

    return 0;
}
