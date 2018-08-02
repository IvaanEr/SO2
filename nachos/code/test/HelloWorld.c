#include "syscall.h"

int main() {
    char *args[2];
    args[0] = "../test/hola";
    args[1] = 0;
    int pid = Exec("../test/hola", args);
    int pid2 = Exec("../test/hola", args);
    Join(pid);
    Join(pid2); 
    Exit(0);
}
