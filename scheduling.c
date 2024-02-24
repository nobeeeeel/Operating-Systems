#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int arrival_time;
    int *cpu_bursts;
    int *io_bursts;
    int num_bursts;
    int current_burst_index;
    int remaining_cpu_time;
    int remaining_io_time;
    int finished;
    int completion_time; // New field to store completion time
} Process;

void performScheduling(Process processes[], int num_processes) {
    int current_time = 0;
    int completed_processes = 0;
    int *turnaround_times = (int *)malloc(num_processes * sizeof(int));

    while (completed_processes < num_processes) {
        int next_process_index = -1;
        int min_arrival_time = __INT_MAX__;

        // Find the next process to execute
        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].finished && processes[i].arrival_time <= current_time && processes[i].arrival_time < min_arrival_time) {
                next_process_index = i;
                min_arrival_time = processes[i].arrival_time;
            }
        }

        if (next_process_index != -1) {
            printf("Executing CPU burst for process %d\n", next_process_index);
            processes[next_process_index].remaining_cpu_time--;
            current_time++;

            // If CPU burst is completed
            if (processes[next_process_index].remaining_cpu_time == 0) {
                printf("Process %d finished its CPU burst\n", next_process_index);
                processes[next_process_index].current_burst_index++;
                if (processes[next_process_index].current_burst_index == processes[next_process_index].num_bursts * 2) { // Updated condition
                    printf("Process %d finished all bursts\n", next_process_index);
                    processes[next_process_index].finished = 1;
                    processes[next_process_index].completion_time = current_time; // Set completion time
                    turnaround_times[next_process_index] = current_time - processes[next_process_index].arrival_time;
                    completed_processes++;
                } else if (processes[next_process_index].current_burst_index % 2 == 0) { // If CPU burst just completed
                    printf("Switching to I/O for process %d\n", next_process_index);
                    int io_burst_index = processes[next_process_index].current_burst_index / 2;
                    processes[next_process_index].remaining_io_time = processes[next_process_index].io_bursts[io_burst_index];
                } else { // If I/O burst just completed
                    printf("Switching to CPU for process %d\n", next_process_index);
                    int cpu_burst_index = processes[next_process_index].current_burst_index / 2;
                    processes[next_process_index].remaining_cpu_time = processes[next_process_index].cpu_bursts[cpu_burst_index];
                }
            }
        } else {
            current_time++;
        }
    }

    // Calculate average turnaround time
    double total_turnaround_time = 0.0;
    for (int i = 0; i < num_processes; i++) {
        total_turnaround_time += turnaround_times[i];
    }
    double avg_turnaround_time = total_turnaround_time / num_processes;
    printf("Average turnaround time: %.2lf\n", avg_turnaround_time);

    free(turnaround_times);
}

int main() {
    // Process 1
    int cpu_bursts_1[] = {100, 50, 100, 100, 200};
    int io_bursts_1[] = {25, 20, 20, 10, 0};
    int arrival_time_1 = 0;
    int num_bursts_1 = sizeof(cpu_bursts_1) / sizeof(cpu_bursts_1[0]);

    // Process 2
    int cpu_bursts_2[] = {30, 30, 40, 50};
    int io_bursts_2[] = {15, 10, 10, 0};
    int arrival_time_2 = 15;
    int num_bursts_2 = sizeof(cpu_bursts_2) / sizeof(cpu_bursts_2[0]);

    // Initialize processes
    Process processes[] = {
        {arrival_time_1, cpu_bursts_1, io_bursts_1, num_bursts_1, 0, cpu_bursts_1[0], io_bursts_1[0], 0, 0},
        {arrival_time_2, cpu_bursts_2, io_bursts_2, num_bursts_2, 0, cpu_bursts_2[0], io_bursts_2[0], 0, 0}
    };

    // Perform scheduling
    performScheduling(processes, 2);

    return 0;
}
