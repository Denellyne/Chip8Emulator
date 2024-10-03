#include "renderer.h"
#include <iostream>

renderer::renderer() {
  SDL_Init(SDL_INIT_EVERYTHING);
  if (SDL_CreateWindowAndRenderer(64, 32, SDL_WINDOW_RESIZABLE, &window,
                                  &rend) == -1) {
    std::cerr << "Window not created, exiting program\n";
    exit(-1);
  }
  SDL_SetWindowTitle(window, "Chip8");
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
}

void renderer::draw(bool display[]) {
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  SDL_Rect rc;
  rc.x = 0;
  rc.y = 0;
  rc.w = 1;
  rc.h = 1;
  SDL_RenderClear(rend);
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
  for (int y = 0; y < 32; y++) {
    rc.x = 0;
    for (int x = 0; x < 64; x++) {
      if (display[(64 * y) + x])
        SDL_RenderFillRect(rend, &rc);
      rc.x++;
    }
    rc.y++;
  }

  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

  SDL_RenderSetScale(rend, w / 64.0f, h / 32.0f);
  SDL_RenderPresent(rend);
}

void renderer::updateKeyboard(bool keypad[]) {
  state = SDL_GetKeyboardState(nullptr);
  memset(keypad, 0, 16);

  if (state[SDL_SCANCODE_1])
    keypad[0] = true;
  if (state[SDL_SCANCODE_2])
    keypad[1] = true;
  if (state[SDL_SCANCODE_3])
    keypad[2] = true;
  if (state[SDL_SCANCODE_4])
    keypad[3] = true;
  if (state[SDL_SCANCODE_Q])
    keypad[4] = true;
  if (state[SDL_SCANCODE_W])
    keypad[5] = true;
  if (state[SDL_SCANCODE_E])
    keypad[6] = true;
  if (state[SDL_SCANCODE_R])
    keypad[7] = true;
  if (state[SDL_SCANCODE_A])
    keypad[8] = true;
  if (state[SDL_SCANCODE_S])
    keypad[9] = true;
  if (state[SDL_SCANCODE_D])
    keypad[10] = true;
  if (state[SDL_SCANCODE_F])
    keypad[11] = true;
  if (state[SDL_SCANCODE_Z])
    keypad[12] = true;
  if (state[SDL_SCANCODE_X])
    keypad[13] = true;
  if (state[SDL_SCANCODE_C])
    keypad[14] = true;
  if (state[SDL_SCANCODE_V])
    keypad[15] = true;
}

void renderer::pollEvents() {
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      exit(0);
      break;
    default:
      break;
    }
  }
}

renderer::~renderer() {

  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
