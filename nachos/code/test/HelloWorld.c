#include "syscall.h"

int main() {
    char **args = {"../test/hola", 0};
    int pid = Exec("../test/hola", args);
    Join(pid);
    Exit(450);
}
