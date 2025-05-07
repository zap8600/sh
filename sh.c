#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* input = NULL;
    long long int ccount = 0;
    int ch = EOF;
    while((ch = fgetc(stdin)) != '\n') {
        input = (char*)realloc(input, ++ccount);
        input[ccount - 1] = (char)ch;
    }
    input = (char*)realloc(input, ccount + 1);
    input[ccount] = '\0';
    puts(input);
    free(input);
    return 0;
}