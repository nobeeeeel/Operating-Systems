#include <stdio.h>
#include <stdlib.h>

// Function to check if a page is present in memory
int isInMemory(int page, int *memory, int num_frames) {
    for (int i = 0; i < num_frames; i++) {
        if (memory[i] == page) {
            return 1;
        }
    }
    return 0;
}

int clock(int *page_references, int num_references, int num_frames) {
    if (num_references == 0) {
        return 0; // No page faults if there are no page references
    }
    if (num_frames <= 0) {
        printf("Invalid number of frames\n");
        return -1; // Invalid input, exit with error code
    }

    int page_faults = 0;
    int *memory = (int *)malloc(num_frames * sizeof(int));
    if (memory == NULL) {
        printf("Memory allocation failed\n");
        return -1; // Memory allocation failed, exit with error code
    }

    // Initialize memory contents to -1 (indicating no page loaded)
    for (int i = 0; i < num_frames; i++) {
        memory[i] = -1;
    }

    int hand = 0; // Clock hand for the algorithm

    for (int i = 0; i < num_references; i++) {
        if (!isInMemory(page_references[i], memory, num_frames)) {
            // Page fault occurred
            memory[hand] = page_references[i];
            hand = (hand + 1) % num_frames; // Move the hand forward
            page_faults++;
        }
    }

    free(memory);
    return page_faults;
}

int main() {
    int num_frames;
    if (scanf("%d", &num_frames) != 1) {
        printf("Invalid input\n");
        return 1; // Invalid input, exit with error code
    }

    int page_references[10000]; // Assuming maximum 10000 references
    int num_references = 0;
    int page;

    // Read page references until maximum capacity is reached or until end of input
    while (num_references < 10000 && scanf("%d", &page_references[num_references]) == 1) {
        num_references++;
    }

    int faults = clock(page_references, num_references, num_frames);
    if (faults >= 0) {
        printf("%d\n", faults);
        return 0;
    } else {
        return 1; // Error occurred, exit with error code
    }
}
