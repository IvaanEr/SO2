
#include "syscall.h"

int main(){
	char **args1 = {"../test/PrintA", 0};
	char **args2 = {"../test/PrintB", 0};
	char **args3 = {"../test/PrintC", 0};
	int i = Exec("../test/PrintA",args1);
	int j = Exec("../test/PrintB",args2);
	int k = Exec("../test/PrintC",args3);

  Join(i);
  Join(j);
  Join(k);
  Exit(1);

}
