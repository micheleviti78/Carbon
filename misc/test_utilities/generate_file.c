#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main() {
    // File name and block size
    const char *filename = "output.bin";
    const size_t block_size = 512;
    const size_t num_blocks =
        2000; // Change this to the desired number of blocks

    // Open the file in binary write mode
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Buffer for each 512-byte block
    uint8_t block[block_size];
    uint8_t index = 0;

    // Generate each block
    for (unsigned i = 0; i < num_blocks; ++i) {
        // Fill the block according to the pattern

        printf("writing index %u \n\r", i);

        // 27 bytes of 0x00
        memset(block, 0x00, 27);

        // 99 bytes of 0xAE
        memset(block + 27, 0xAE, 99);

        // 129 bytes of the current index
        memset(block + 126, index, 129);

        // 70 bytes of 0xAA
        memset(block + 255, 0xAA, 70);

        // The remaining 187 bytes of 0x01
        memset(block + 325, 0x01, 187);

        index++;

        // Write the block to the file
        if (fwrite(block, 1, block_size, file) != block_size) {
            perror("Error writing to file");
            fclose(file);
            return 1;
        }
    }

    // Close the file
    fclose(file);

    printf("Binary file '%s' created with %zu blocks of 512 bytes each\n",
           filename, num_blocks);
    return 0;
}
