#include "tydos.h"

int main()
{
  puts("Tell me you name: ");

  char buffer[10];
  gets(buffer);

  puts("Hello ");
  puts(buffer);
  puts("!\n");
  return 0;
}