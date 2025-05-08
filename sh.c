#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <setjmp.h>

static sigjmp_buf env; // Never used setjmp and longjmp
static volatile sig_atomic_t jumpact = 0;

void siginthd(int signo) {
    if(!jumpact) {
        return;
    }

    siglongjmp(env, 1);
}

int main() {
    char* input = NULL;
    char** command = NULL;
    unsigned long int ccount = 0;
    unsigned long int cmcount = 0;
    int ch = EOF;
    struct sigaction s;
    struct sigaction schild;
    s.sa_handler = siginthd;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    sigaction(SIGINT, &s, &schild);
    while(1) {
        if(sigsetjmp(env, 1)) {
            putchar('\n');
        }
        jumpact = 1;
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
        char* parsed = strtok(input, " "); // First time using this too
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
                sigaction(SIGINT, &schild, NULL);
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
