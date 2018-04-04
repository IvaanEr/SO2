#include "syscall.h"

int main() {
    char *args[2];
    args[0] = "../test/hola";
    args[1] = 0;
    int pid = Exec("../test/hola", args);
    Join(pid); 
    Exit(0);
}
