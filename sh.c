#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main() {
    char* input = NULL;
    char** command = NULL;
    long long int ccount = 0;
    long long int cmcount = 0;
    int ch = EOF;
    while(1) {
        while((ch = fgetc(stdin)) != '\n') {
            input = (char*)realloc(input, ++ccount);
            input[ccount - 1] = (char)ch;
        }
        input = (char*)realloc(input, ccount + 1);
        input[ccount] = '\0';
        char* cch = input;
        char* ccmd = input;
        while(1) {
            if(*cch == ' ' || *cch == '\0') {
                *cch++ = '\0';
                command = (char**)realloc(command, ++cmcount * sizeof(char*));
                command[cmcount - 1] = ccmd;
                ccmd = cch;
            }
        }
    }
    return 0;
}