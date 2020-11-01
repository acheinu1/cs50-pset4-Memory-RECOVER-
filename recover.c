#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BLOCKSIZE 512

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // validate input
    if (argc != 2)
    {
        fprintf(stderr, "Usage: recover infile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];

    FILE *inputPointer = fopen(infile, "r");
    if (inputPointer == NULL)
    {
        fprintf(stderr, "error Couldnt find file %s.\n", infile);
        return 2;
    }

    BYTE buffer[512];
    int imageCount = 0;

    char filename[8];
    FILE *outputPointer = NULL;

    while (true)
    {
        // read a block of the memory card image
        size_t bytesRead = fread(buffer, sizeof(BYTE), BLOCKSIZE, inputPointer);

        // break out of the loop when we reach the end of the card image
        if (bytesRead == 0 && feof(inputPointer))
        {
            break;
        }

        // check if we found a JPEG
        bool containsJpegHeader = buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0;

        // if we found a yet another JPEG, we must close the previous file
        if (containsJpegHeader && outputPointer != NULL)
        {
            fclose(outputPointer);
            imageCount++;
        }

        // write in any found jpeg
        if (containsJpegHeader)
        {
            sprintf(filename, "%03i.jpg", imageCount);
            outputPointer = fopen(filename, "w");
        }

        // write anytime we have an open file
        if (outputPointer != NULL)
        {
            fwrite(buffer, sizeof(BYTE), bytesRead, outputPointer);
        }
    }

    
    fclose(outputPointer);


    fclose(inputPointer);

    // success
    return 0;
}