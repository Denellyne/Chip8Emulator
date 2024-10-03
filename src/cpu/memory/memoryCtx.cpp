#include "memoryCtx.h"

void memoryCtx::stackPush(short data) {
  stack[stackPtr++] = data;
  assert(stackPtr < 16);
}
short memoryCtx::stackPop() { return stack[stackPtr--]; }

u16 memoryCtx::fetch() {
  u16 instruction = ((memory[PC] << 8) | memory[PC + 1]);
  PC += 2;
  return instruction;
}

bool memoryCtx::getFlag(char idx) { return ((registers[15] >> idx) & 1); }

void memoryCtx::setFlag(char idx, bool set) {

  if (set)
    registers[15] |= 1 << idx;
  else
    registers[15] &= ~(1 << idx);
}

void memoryCtx::writeToRegister(u8 data, u8 idx) { registers[idx] = data; }

u8 memoryCtx::readFromRegister(u8 idx) { return registers[idx]; }

u16 memoryCtx::readFromRegister(u8 idxF, u8 idxS) {
  return ((registers[idxF] << 8) | registers[idxS]);
}

void memoryCtx::returnSubroutine(){
  stackPtr--;
  PC = stack[stackPtr];
}
