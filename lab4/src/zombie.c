#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


int main(int argc, char *argv[]){
    pid_t child_pid = fork();
    if (child_pid == 0) {
        printf("it will be zombie process\n");
        return 0;
    }
    sleep(2);

}