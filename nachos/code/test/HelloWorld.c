#include "syscall.h"

int main() {
    char **args = {"../test/hola", 0};
    int pid = Exec("../test/hola", args);
    Join(pid); // Para este programa en particular el Join no está andando. Por qué? En consoleTest sí anda.
    Exit(0);
}
