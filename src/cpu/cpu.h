#pragma once
#include "memory/memoryCtx.h"
#include "renderer/renderer.h"
#include <cstddef>
#include <cstring>
#include <ctime>
#include <random>
#include <string>
#include <thread>
#include <windows.h>

class Cpu {

public:
  Cpu();
  Cpu(std::string path);

  ~Cpu();

  void reset() {
    memory.resetMemory();
    memset(display, 0, 64 * 32);
    memset(keypad, 0, 16);
    srand(time(NULL));
    shouldDraw = false;
  }

  void run();

private:
  void timers();
  void draw();
  void loadRom(std::string path);

  void execute();
  void debug(int instruction);
  void startSound();
  void stopSound();

  memoryCtx memory;
  renderer gfx;
  u8 rom[4096];
  bool display[64 * 32];
  bool keypad[16];
  bool shouldDraw;
};
