#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE (1024 * 1024)  /* 1 MB hard limit for Phase 1 */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <html-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    static char buffer[MAX_SIZE];  /* static so it lives in BSS, not the stack */
    size_t n = fread(buffer, 1, MAX_SIZE - 1, fp);
    fclose(fp);

    buffer[n] = '\0';

    if (n == 0) {
        fprintf(stderr, "Error: file is empty\n");
        return EXIT_FAILURE;
    }

    /* Print the file contents */
    printf("%s", buffer);
    if (buffer[n - 1] != '\n')
        printf("\n");

    return EXIT_SUCCESS;
}
