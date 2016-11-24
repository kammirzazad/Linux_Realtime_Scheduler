#include <unistd.h>
#include <stdio.h>

int main()
{
  uid_t x = fork();

  printf(" ( %i ) \n" , x );

  return 0;
}
