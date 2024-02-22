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

int fifo(int *page_references, int num_references, int num_frames) {
    int page_faults = 0;
    int *memory = (int *)malloc(num_frames * sizeof(int));

    // Initialize memory contents to -1 (indicating no page loaded)
    for (int i = 0; i < num_frames; i++) {
        memory[i] = -1;
    }

    int next_replace_index = 0;

    for (int i = 0; i < num_references; i++) {
        if (!isInMemory(page_references[i], memory, num_frames)) {
            memory[next_replace_index] = page_references[i];
            next_replace_index = (next_replace_index + 1) % num_frames;
            page_faults++;
        }
    }

    free(memory);
    return page_faults;
}

int main() {
    int num_frames;
    scanf("%d", &num_frames);

    int page_references[100]; // Assuming maximum 100 references
    int num_references = 0;
    int page;

    // Read page references
    while (scanf("%d", &page) == 1) {
        page_references[num_references++] = page;
    }

    int faults = fifo(page_references, num_references, num_frames);
    printf("%d\n", faults);

    return 0;
}
