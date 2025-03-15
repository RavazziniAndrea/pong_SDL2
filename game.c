#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define WINDOW_TITLE "PONG"
#define SCREEN_W 800
#define SCREEN_H 600

int real_width, real_height;

struct Game {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Surface *screen;
  SDL_Texture *texture;
};

struct Bar {
  int x;
  int y;
  int w;
  int h;
};

struct Ball {
  int x;
  int y;
  int w;
  int h;
  int dx;
  int dy;
};

void game_cleanup(struct Game *game, int exitstatus) {
  SDL_FreeSurface(game->screen);
  SDL_DestroyTexture(game->texture);
  SDL_DestroyRenderer(game->renderer);
  SDL_DestroyWindow(game->window);
  SDL_Quit();

  exit(exitstatus);
}

int sdl_initialize(struct Game *game) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stderr, "Init failed! %s\n", SDL_GetError());
    return -1;
  }

  game->window =
      SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
  if (!game->window) {
    fprintf(stderr, "Window init failed! %s\n", SDL_GetError());
    return -1;
  }

  game->renderer = SDL_CreateRenderer(game->window, -1, 0);
  if (!game->renderer) {
    fprintf(stderr, "Render init failed! %s\n", SDL_GetError());
    return -1;
  }

  game->screen = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H, 32,
                                                SDL_PIXELFORMAT_RGBA32);
  if (!game->screen) {
    fprintf(stderr, "Screen init failed! %s\n", SDL_GetError());
    return -1;
  }

  game->texture = SDL_CreateTextureFromSurface(game->renderer, game->screen);
  if (!game->texture) {
    fprintf(stderr, "Texture init failed! %s\n", SDL_GetError());
    return -1;
  }

  return 0;
}

/**
 * TODO LIST
 *  - create entities
 *  - setup window
 *  - make it run
 *  - implement bars movement
 *  - implement ball physics
 *  - implement game logic
 *  - refactor and separation
 *
 */
int main(void) {
  printf("Start...\n");

  struct Game game = {
      .window = NULL,
      .renderer = NULL,
  };

  if (sdl_initialize(&game)) {
    game_cleanup(&game, EXIT_FAILURE);
  }

  SDL_GetWindowSize(game.window, &real_width, &real_height);

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
          running = false;
          break;
        default:
          printf("Pressed: %c\n", event.key.keysym.sym);
          break;
        }
      }

      SDL_RenderClear(game.renderer);
      SDL_RenderPresent(game.renderer);
      SDL_Delay(16);
    }
  }

  printf("Done\n");
  game_cleanup(&game, EXIT_SUCCESS);
  return 0;
}
