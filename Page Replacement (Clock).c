#include <stdio.h>
#include <stdlib.h>

// Function to check if a page is present in memory
int isInMemory(int page, int *memory, int *use_bits, int num_frames) {
    for (int i = 0; i < num_frames; i++) {
        if (memory[i] == page) {
            // Mark the corresponding use bit as 1
            use_bits[i] = 1;
            return 1;
        }
    }
    return 0;
}

int clock(int *page_references, int num_references, int num_frames) {
    if (num_references == 0) {
        return 0; // No page faults if there are no page references
    }
    if (num_frames <= 0 || num_references > 10000) {
        printf("Invalid input\n");
        return -1; // Invalid input, exit with error code
    }

    int page_faults = 0;
    int *memory = (int *)malloc(num_frames * sizeof(int));
    int *use_bits = (int *)malloc(num_frames * sizeof(int));
    if (memory == NULL || use_bits == NULL) {
        printf("Memory allocation failed\n");
        return -1; // Memory allocation failed, exit with error code
    }

    // Initialize memory contents and use bits to 0
    for (int i = 0; i < num_frames; i++) {
        memory[i] = -1;
        use_bits[i] = 0;
    }

    int hand = 0; // Clock hand for the algorithm

    for (int i = 0; i < num_references; i++) {
        if (!isInMemory(page_references[i], memory, use_bits, num_frames)) {
            // Page fault occurred
            while (use_bits[hand] == 1) {
                use_bits[hand] = 0; // Reset the use bit
                hand = (hand + 1) % num_frames; // Move the hand forward
            }
            // Replace the page at the hand position
            memory[hand] = page_references[i];
            use_bits[hand] = 1;
            hand = (hand + 1) % num_frames; // Move the hand forward
            page_faults++;
        }
    }

    free(memory);
    free(use_bits);
    return page_faults;
}

int main() {
    int num_frames;
    if (scanf("%d", &num_frames) != 1) {
        printf("Invalid input\n");
        return 1; // Invalid input, exit with error code
    }
    if (num_frames <= 0 || num_frames > 10) {
        printf("Invalid number of frames\n");
        return 1; // Invalid input, exit with error code
    }

    int page_references[100]; // Assuming maximum 100 references
    int num_references = 0;
    int page;

    // Read page references until a newline is encountered or maximum capacity is reached
    while (num_references < 100 && scanf("%d", &page) == 1) {
        if (page < 0 || page > 50) {
            printf("Invalid page number. Please enter a number between 0 and 50.\n");
            return 1; // Invalid input, exit with error code
        }
        page_references[num_references++] = page;
        char c = getchar(); // Read the next character
        if (c == '\n') {
            break; // Stop reading if newline is encountered
        }
    }

    int faults = clock(page_references, num_references, num_frames);
    printf("%d\n", faults);

    return 0;
}