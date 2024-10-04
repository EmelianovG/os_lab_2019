#include <stdarg.h>
#include <stdio.h>
int main(){
    execl("sequential.elf", "sequential.elf", "10", "100", NULL);
    return 1;
}