#include <SDL.h>
#include <stdio.h>

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) { printf("SDL init err\n"); return 1; }
  SDL_Window* win = SDL_CreateWindow("WASM OK", 100, 100, 640, 360, 0);
  SDL_Renderer* r = SDL_CreateRenderer(win, -1, 0);
  for (int i=0;i<300;i++){ SDL_SetRenderDrawColor(r, 5, 120, 60, 255); SDL_RenderClear(r); SDL_RenderPresent(r); SDL_Delay(16); }
  SDL_Quit();
  return 0;
}
