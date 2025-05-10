#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>

int main() {
    char* input = NULL;
    char** command = NULL;
    unsigned long int ccount = 0;
    unsigned long int cmcount = 0;
    int ch = EOF;
    signal(SIGINT, SIG_IGN);
    while(1) {
        fputs("> ", stdout);
        while((ch = fgetc(stdin)) != '\n') {
            if(ch == EOF) {
                putchar('\n');
                exit(1);
            }
            input = (char*)realloc(input, ++ccount);
            input[ccount - 1] = (char)ch;
        }
        input = (char*)realloc(input, ccount + 1);
        input[ccount] = '\0';
        char* parsed = strtok(input, " "); // First time using this
        while(parsed) {
            command = (char**)realloc(command, ++cmcount * sizeof(char*));
            command[cmcount - 1] = parsed;
            parsed = strtok(NULL, " ");
        }
        char* fileout = NULL;
        for(unsigned long int i = 1; i < cmcount; i++) {
            if(!strcmp(command[i], ">")) {
                fileout = command[i + 1];
                cmcount -= 2;
                command = (char**)realloc(command, cmcount * sizeof(char*));
                break;
            }
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
            int filefd;
            if(fileout) {
                filefd = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            }
            pid_t child = fork();
            if(child < 0) {
                perror("failed to fork");
                exit(1);
            }
            if(!child) {
                signal(SIGINT, SIG_DFL);
                if(fileout) {
                    dup2(filefd, 1);
                }
                if(execvp(command[0], command) < 0) {
                    perror(command[0]);
                    exit(1);
                }
            } else {
                int stateloc;
                waitpid(child, &stateloc, WUNTRACED);
            }
            close(filefd);
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
