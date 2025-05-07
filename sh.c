#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* input;
    while(1) {
        int ch = EOF;
        while((ch = fgetc(stdin)) != '\n') {
            if(ch != EOF)
        }
    }
}