#include "syscall.h"

int main() {
    char **argv = {"hola", 0};
    int pid = Exec("../test/hola", argv);
    Join(pid);
    // Exit(0);
}
