#include "cpu.h"
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <playsoundapi.h>
#include <thread>
#include <winuser.h>

Cpu::Cpu() { reset(); }
Cpu::Cpu(std::string path) {
  reset();
  loadRom(path);
  run();
}

Cpu::~Cpu() {}

void Cpu::loadRom(std::string path) {
  memset(rom, 0, 4096);
  std::ifstream loader(path, std::fstream::binary);
  int idx = 0x200;
  u8 byte;
  std::cout << "Loading rom...\n";
  while (loader.good()) {
    byte = loader.get();
    rom[idx++] = byte;
  }
  loader.close();

  memcpy(memory.memory, rom, 4096);
}

void Cpu::debug(int instruction) {
  for (int i = 0; i < 16; i++)
    std::cout << "Reg " << i << ": " << (int)memory.registers[i] << ' ';

  printf("PC:\n%i \n", memory.PC);
  printf("SP:%i \n", memory.stackPtr);
  printf("I:%i \n", memory.I);
  printf("Delay:%i \n", memory.delay);
  printf("Sound:%i \n", memory.sound);
  printf("Instruction = 0x%04x\n", instruction);

  for (const auto &key : keyboard)
    std::cout << keypad[key.first] << " \n";
}

void Cpu::draw() { gfx.draw(display); }

void Cpu::execute() {

#define Opcode instructionNibbles[0]
#define Vx memory.registers[instructionNibbles[1]]
#define Vy memory.registers[instructionNibbles[2]]
#define VF memory.registers[15]

  std::array<int, 4> instructionNibbles;
  u16 instruction = 1;

  instruction = memory.fetch();
  instructionNibbles[0] = (instruction >> 12) & 0x000F;
  instructionNibbles[1] = (instruction >> 8) & 0x000F;
  instructionNibbles[2] = (instruction >> 4) & 0x000F;
  instructionNibbles[3] = instruction & 0x000F;

  // debug(instruction);
  gfx.updateKeyboard(keypad);
  switch ((int)Opcode) {

  case 0:
    if ((int)instructionNibbles[3] == 0)
      memset(display, 0, 64 * 32);
    else
      memory.returnSubroutine();
    break;
  case 1:
    memory.PC = (u16)((instructionNibbles[1] << 8) |
                      (instructionNibbles[2] << 4) | instructionNibbles[3]);
    break;

  case 2:
    memory.stack[memory.stackPtr] = memory.PC;
    memory.stackPtr++;
    memory.PC = (u16)((instructionNibbles[1] << 8) |
                      (instructionNibbles[2] << 4) | instructionNibbles[3]);
    assert(memory.stackPtr < 16);
    break;

  case 3:
    if (Vx == int((instructionNibbles[2] << 4) | instructionNibbles[3]))
      memory.PC += 2;
    break;

  case 4:
    if (Vx != int((instructionNibbles[2] << 4) | instructionNibbles[3]))
      memory.PC += 2;
    break;

  case 5:
    if (Vx == Vy)
      memory.PC += 2;
    break;

  case 6:
    Vx = int((instructionNibbles[2] << 4) | instructionNibbles[3]);
    break;

  case 7:
    Vx += int((instructionNibbles[2] << 4) | instructionNibbles[3]);
    break;

  case 8:
    if ((int)instructionNibbles[3] == 0)
      Vx = Vy;

    else if (instructionNibbles[3] == 1)
      Vx |= Vy;

    else if (instructionNibbles[3] == 2)
      Vx &= Vy;
    else if (instructionNibbles[3] == 3)
      Vx ^= Vy;
    else if (instructionNibbles[3] == 4) {

      if (Vx + Vy > 255)
        VF = 1;
      else
        VF = 0;
      Vx += Vy;
    }

    else if (instructionNibbles[3] == 5) {

      if (Vx > Vy)
        VF = 1;
      else
        VF = 0;
      Vx -= Vy;
    }

    else if (instructionNibbles[3] == 6) {
      VF = Vx & 1;
      Vx >>= 1;
    }

    else if (instructionNibbles[3] == 7) {

      if (Vx > Vy)
        VF = 1;
      else
        VF = 0;
      Vx = Vy - Vx;
    }

    else if (instructionNibbles[3] == 0xE) {
      VF = Vx >> 7;
      Vx <<= 1;
    }
    break;

  case 9:
    if (Vx != Vy)
      memory.PC += 2;
    break;

  case 10:
    memory.I = (instructionNibbles[1] << 8) | (instructionNibbles[2] << 4) |
               instructionNibbles[3];
    break;
  case 11:
    memory.PC = ((instructionNibbles[1] << 8) | (instructionNibbles[2] << 4) |
                 instructionNibbles[3]) +
                memory.registers[0];
    break;
  case 12:
    Vx = ((rand() % 255) &
          ((instructionNibbles[2] << 4) | instructionNibbles[3]));
    break;

  case 13: {

    VF = false;
    int x = Vx;
    int y = Vy;
    int bytes = instructionNibbles[3];
    for (int i = 0; i < bytes; i++) {
      u8 lineData = memory.memory[memory.I + i];
      for (int j = 0; j < 8; j++) {
        if (y + i >= 32 || x + j >= 64)
          continue;

        if ((lineData & (0x80 >> j)) && (display[((y + i) * 64) + x + j])) {

          VF = true;
          display[((y + i) * 64) + x + j] = false;
        } else
          display[((y + i) * 64) + x + j] ^= lineData & (0x80 >> j);
      }
    }
    shouldDraw = true;
  } break;

  case 14:
    if (instructionNibbles[3] == 1 && !keypad[Vx])
      memory.PC += 2;
    else if (instructionNibbles[3] == 0xE && keypad[Vx])
      memory.PC += 2;

    break;

  case 15:
    int lowNibbles = (instructionNibbles[2] << 4) | instructionNibbles[3];
    switch (lowNibbles) {
    case 0x07:
      Vx = memory.delay;
      break;
    case 0x0A: {
      while (1) {
        gfx.updateKeyboard(keypad);
        for (const auto &key : keyboard) {
          if (keypad[key.first]) {
            Vx = key.first;
            return;
          }
        }
      }
      break;
    }
    case 0x15:
      memory.delay = Vx;
      break;
    case 0x18:
      memory.sound = Vx;
      startSound();
      break;
    case 0x1E:
      memory.I += Vx;
      break;
    case 0x29:
      memory.I = Vx * 5;
      break;
    case 0x33:
      memory.memory[memory.I + 2] = Vx % 10;
      memory.memory[memory.I + 1] = (Vx / 10) % 10;
      memory.memory[memory.I] = (Vx / 100);
      break;
    case 0x55:
      for (int i = 0; i <= instructionNibbles[1]; i++)
        memory.memory[memory.I + i] = memory.registers[i];
      break;
    case 0x65:

      for (int i = 0; i <= instructionNibbles[1]; i++)
        memory.registers[i] = memory.memory[memory.I + i];
      break;
    }

    break;
  }
}

void Cpu::startSound() { PlaySoundA("beep.wav", NULL, SND_LOOP | SND_ASYNC); }

void Cpu::stopSound() { PlaySoundA(NULL, NULL, SND_LOOP); }

void Cpu::timers() {
  while (1) {

    const auto startTimePoint = std::chrono::high_resolution_clock::now();

    if (memory.delay > 0)
      memory.delay--;
    if (memory.sound > 0)
      memory.sound--;
    if (memory.sound == 0)
      stopSound();

    const auto endTimePoint = std::chrono::high_resolution_clock::now();
    auto start =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(startTimePoint)
            .time_since_epoch()
            .count();
    auto end =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimePoint)
            .time_since_epoch()
            .count();

    unsigned long long duration = (end - start);
    std::this_thread::sleep_for(std::chrono::nanoseconds(30000000 - duration));
  }
}
void Cpu::run() {
  std::thread timer(&Cpu::timers, this);
  timer.detach();
  while (1) {

    const auto startTimePoint = std::chrono::high_resolution_clock::now();

    gfx.pollEvents();
    execute();

    if (shouldDraw) {
      shouldDraw = false;
      gfx.draw(display);
    }

    const auto endTimePoint = std::chrono::high_resolution_clock::now();
    auto start =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(startTimePoint)
            .time_since_epoch()
            .count();
    auto end =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimePoint)
            .time_since_epoch()
            .count();

    unsigned long long duration = (end - start);
    std::this_thread::sleep_for(std::chrono::nanoseconds(1100000 - duration));
  }
}
