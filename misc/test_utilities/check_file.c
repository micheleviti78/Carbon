#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 512

static uint8_t index_value = 0;
static uint32_t global_index = 0;

bool check_block(const uint8_t *block) {
    bool isOK = true;

    if (block == NULL) {
        fprintf(stderr, "Invalid block pointer\n");
        isOK = false;
    }

    // Check the first 27 bytes are 0x00
    for (size_t i = 0; i < 27; ++i) {
        if (block[i] != 0x00) {
            // printf("Error: Byte %zu is not 0x00, but %u\n", i, block[i]);
            isOK = false;
        }
    }

    // Check the next 99 bytes are 0xAE
    for (size_t i = 27; i < 126; ++i) {
        if (block[i] != 0xAE) {
            // printf("Error: Byte %zu is not 0xAE, but %u\n", i, block[i]);
            isOK = false;
        }
    }

    // Check the next 129 bytes have the same index value
    for (size_t i = 126; i < 255; ++i) {
        if (block[i] != index_value) {
            // printf("Error: Byte %zu is not equal to index value %u, but
            // %u\n", i, index_value, block[i]);
            isOK = false;
        }
    }

    index_value++;

    // Check the next 70 bytes are 0xAA
    for (size_t i = 255; i < 325; ++i) {
        if (block[i] != 0xAA) {
            // printf("Error: Byte %zu is not 0xAA, but %u\n", i, block[i]);
            isOK = false;
        }
    }

    // Check the remaining 187 bytes are 0x01
    for (size_t i = 325; i < BLOCK_SIZE; ++i) {
        if (block[i] != 0x01) {
            // printf("Error: Byte %zu is not 0x01, but %u\n", i, block[i]);
            isOK = false;
        }
    }

    if (isOK) {
        // If all checks passed, the block is valid
        return true;
    } else {
        return false;
    }
}

int main() {
    const char *filename = "output.bin";

    // Open the generated binary file in binary read mode
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Buffer to store a single block of 512 bytes
    uint8_t block[BLOCK_SIZE];
    size_t block_num = 0;

    // Read and check each block in the file
    while (fread(block, 1, BLOCK_SIZE, file) == BLOCK_SIZE) {
        if (block_num % 20 == 0) {
            printf("Checking block %zu...\n", block_num);
        }

        // Use check_block to validate the current block
        if (!check_block(block)) {
            printf("Block %zu is invalid.\n", block_num);
        }

        block_num++;
    }

    // Close the file
    fclose(file);

    printf("Finished checking %zu blocks in '%s'.\n", block_num, filename);
    return 0;
}
