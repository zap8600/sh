#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

int main() {
    char* input = NULL;
    char** command = NULL;
    unsigned long long int ccount = 0;
    unsigned long long int cmcount = 0;
    int ch = EOF;
    signal(SIGINT, SIG_IGN);
    while(1) {
        fputs("> ", stdout);
        while((ch = fgetc(stdin)) != '\n') {
            input = (char*)realloc(input, ++ccount);
            input[ccount - 1] = (char)ch;
        }
        input = (char*)realloc(input, ccount + 1);
        input[ccount] = '\0';
        char* parsed = strtok(input, " ");
        while(parsed) {
            command = (char**)realloc(command, ++cmcount * sizeof(char*));
            command[cmcount - 1] = parsed;
            parsed = strtok(NULL, " ");
        }
        command = (char**)realloc(command, (cmcount + 1) * sizeof(char*));
        command[cmcount] = NULL;
        if(!strcmp(command[0], "cd")) {
            if(cmcount != 1) {
                if(chdir(command[1]) < 0) {
                    perror(command[1]);
                }
            }
        } else {
            pid_t child = fork();
            if(child < 0) {
                perror("failed to fork");
                exit(1);
            }
            if(!child) {
                signal(SIGINT, SIG_DFL);
                if(execvp(command[0], command) < 0) {
                    perror(command[0]);
                    exit(1);
                }
            } else {
                int stateloc;
                waitpid(child, &stateloc, WUNTRACED);
            }
        }
        free(command);
        free(input);
        command = NULL;
        input = NULL;
        ccount = 0;
        cmcount = 0;
    }
    return 0;
}