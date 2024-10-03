#pragma once
#define SDL_MAIN_HANDLED
#include "../../../../SDL2/include/SDL.h"
#undef main

#include <unordered_map>
static std::unordered_map<int, SDL_KeyCode> keyboard{
    {0, SDLK_1},  {1, SDLK_2},  {2, SDLK_3},  {3, SDLK_4},
    {4, SDLK_q},  {5, SDLK_w},  {6, SDLK_e},  {7, SDLK_r},
    {8, SDLK_a},  {9, SDLK_s},  {10, SDLK_d}, {11, SDLK_f},
    {12, SDLK_z}, {13, SDLK_x}, {14, SDLK_c}, {15, SDLK_v}};

class renderer {

public:
  renderer();
  ~renderer();
  void draw(bool display[]);

  void pollEvents();

  void updateKeyboard(bool keypad[]);

private:
  SDL_Window *window = nullptr;
  SDL_Renderer *rend = nullptr;
  SDL_Texture *texture = nullptr;
  SDL_Surface *surface = nullptr;

  SDL_Event event;

  const Uint8 *state = SDL_GetKeyboardState(nullptr);
};
