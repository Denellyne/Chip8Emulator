#include "cpu/cpu.h"
#undef main
int main(int argc, char *argv[]) {
  if (argc > 1)
    Cpu cpu(argv[1]);
  else
    Cpu cpu("test.ch8");
  return 0;
}
