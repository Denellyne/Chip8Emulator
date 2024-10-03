#pragma once

#include <cstdint>
#include <cstring>
#include <cassert>
#define u8 std::uint8_t
#define u16 std::uint16_t

class memoryCtx {
public:
  memoryCtx() { resetMemory();}

  inline void resetMemory() {
    memset(memory, 0, 4096);
    memset(stack, 0, 16);
    memset(registers, 0, 16); 
    stackPtr = 0;
    I = 0;
    delay = 0;
    sound = 0;
    PC = 0x200;
  }

  void setFlag(char idx, bool set);
  bool getFlag(char idx);

  void writeToRegister(u8 data, u8 idx);
  u8 readFromRegister(u8 idx);
  u16 readFromRegister(u8 idxF, u8 idxS);

  void stackPush(short data);
  short stackPop();


  u16 fetch();

  void returnSubroutine();

  u8 memory[4096];

  u8 registers[16];
  
  u16 stack[16];
  u8 stackPtr;

  u16 I;

  u8 delay, sound;

  u16 PC;
};

