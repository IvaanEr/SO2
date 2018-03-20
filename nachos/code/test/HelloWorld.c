#include "syscall.h"

int main() {
    char **argv = {"hola"};
    int pid = Exec("../test/hola", argv);
}
