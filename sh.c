#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/*
Get user input. Caller must free array.
On CTRL+D (EOF), frees array and returns NULL.
*/
char* getuserin() {
    char* r = NULL;
    char c = 0;
    unsigned long int count = 0;
    while(1) {
        int ret = read(0, &c, 1);
        if(ret <= 0) {
            if(r) {
                free(r);
            }
            if(ret < 0) {
                perror("read");
                exit(1);
            }
            return NULL;
        }
        if(c == '\n') break;
        r = (char*)realloc(r, (++count) + 1);
        r[count - 1] = c;
    }
    r[count] = 0;
    return r;
}

/*
Parse an input string into an array of commands and their arguments. Caller must free array.
Does not check if pipes is NULL.
Does not check for command after a pipe, just assumes.
*/
char*** parsecmd(char* s, unsigned long int* pipes) {
    char*** r = NULL;
    *pipes = 0;
    char* parsed = strtok(s, " ");
    unsigned long int count = 0;
    unsigned long int commands = 1;
    r = (char***)malloc(sizeof(char**));
    r[0] = NULL;
    while(parsed) {
        if(!strcmp(parsed, "|")) {
            r[commands - 1][count] = NULL;
            r = (char***)realloc(r, ++commands * sizeof(char**));
            r[commands - 1] = NULL;
            (*pipes)++;
            count = 0;
        } else {
            r[commands - 1] = (char**)realloc(r[commands - 1], ((++count) + 1) * sizeof(char*));
            r[commands - 1][count - 1] = parsed;
        }
        parsed = strtok(NULL, " ");
    }
    return r;
}

const char prompt[2] = {'>', ' '};

int main() {
    char* input = NULL;
    char*** command = NULL;
    unsigned long int pipes = 0;
    signal(SIGINT, SIG_IGN);
    while(1) {
        write(1, prompt, 2);
        if(!(input = getuserin())) {
            putchar('\n');
            exit(1);
        }
        command = parsecmd(input, &pipes);
        if(!strcmp(command[0][0], "cd")) {
            if(chdir(command[0][1]) < 0) {
                perror(command[0][1]);
            }
        } else {
            int** pipefds = NULL;
            if(pipes) {
                pipefds = (int**)malloc(pipes * sizeof(int*));
                for(unsigned long int i = 0; i < pipes; i++) {
                    pipefds[i] = (int*)malloc(2 * sizeof(int));
                }
            }
            for(unsigned long int i = 0; i <= pipes; i++) {
                // TODO: Handle fork error
                if(!fork()) {
                    signal(SIGINT, SIG_DFL);
                    if(pipes) {
                        if(i > 0) {
                            dup2(pipefds[i - 1][0], 0);
                        }
                        if(i < pipes) {
                            dup2(pipefds[i][1], 1);
                        }
                    }
                    if(execvp(command[i][0], command[i]) < 0) {
                        perror(command[i][0]);
                        exit(1);
                    }
                }
            }
            for(unsigned long int i = 0; i <= pipes; i++) {
                wait(NULL);
            }
        }
        for(unsigned long int i = 0; i <= pipes; i++) {
            free(command[i]);
        }
        free(command);
        free(input);
    }
    return 0;
}
