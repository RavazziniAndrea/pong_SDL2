#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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

struct Game game = {
    .window = NULL, .renderer = NULL, .screen = NULL, .texture = NULL};

void game_cleanup(struct Game *game, int exitstatus) {
  SDL_FreeSurface(game->screen);
  SDL_DestroyTexture(game->texture);
  SDL_DestroyRenderer(game->renderer);
  SDL_DestroyWindow(game->window);
  SDL_Quit();

  exit(exitstatus);
}

int sdl_initialize(struct Game *game) {
  if (SDL_Init(SDL_INIT_VIDEO)) {
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

void move_ball(struct Ball *ball) {
  ball->x += ball->dx;
  ball->y += ball->dy;

  if (ball->x < 0 || ball->x > game.screen->w - 10) {
    ball->dx = -ball->dx;
  }
  if (ball->y < 0 || ball->y > game.screen->h - 10) {
    ball->dy = -ball->dy;
  }
}

void draw_ball(struct Ball *ball) {
  SDL_Rect rect;
  rect.x = ball->x;
  rect.y = ball->y;
  rect.w = ball->w;
  rect.h = ball->h;

  SDL_FillRect(game.screen, &rect, 0xffffffff);
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

  if (sdl_initialize(&game)) {
    game_cleanup(&game, EXIT_FAILURE);
  }

  // SDL_GetWindowSize(game.window, &real_width, &real_height);

  struct Ball ball = {
      .x = game.screen->w / 2,
      .y = game.screen->h / 2,
      .h = 10,
      .w = 10,
      .dx = rand() % 10,
      .dy = rand() % 10,
  };

  Uint32 next_tick = SDL_GetTicks();
  int sleep = 0;

  bool running = true;
  while (running) {
    SDL_PumpEvents();
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_ESCAPE]) {
      running = false;
    }

    SDL_RenderClear(game.renderer);
    SDL_FillRect(game.screen, NULL, 0x000000ff);

    move_ball(&ball);
    draw_ball(&ball);

    SDL_UpdateTexture(game.texture, NULL, game.screen->pixels,
                      game.screen->w * sizeof(Uint32));
    SDL_RenderCopy(game.renderer, game.texture, NULL, NULL);
    SDL_RenderPresent(game.renderer);

    next_tick += 1000 / 60;
    sleep = next_tick - SDL_GetTicks();
    if (sleep > 0) {
      SDL_Delay(sleep);
    }
  }

  printf("Done\n");
  game_cleanup(&game, EXIT_SUCCESS);
  return 0;
}
