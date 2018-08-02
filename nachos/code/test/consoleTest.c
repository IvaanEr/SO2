
#include "syscall.h"

int main(){
	char *args1[2];
  args1[0] = "../test/PrintA";
  args1[1] = 0;
	char *args2[2];
  args2[0] = "../test/PrintB";
  args2[1] = 0;
	// char *args3[2];
  // args3[0] = "../test/PrintC";
  // args3[1] = 0;
	int i = Exec("../test/PrintA",args1);
	int j = Exec("../test/PrintB",args2);
	// int k = Exec("../test/PrintC",args3);

  Join(i);
  Join(j);
  // Join(k);
  Exit(1);

}
