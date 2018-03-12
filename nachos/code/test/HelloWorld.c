#include "syscall.h"

int main() {
    char **argv = {"hola", 0};
    Exec("../test/hola", argv);
    Exec("../test/hola", argv);
    Exit(0);
}