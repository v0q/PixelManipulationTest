#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define WIDTH 800
// the height of the screen taking into account the maze and block
#define HEIGHT 600
#define PROJECTILESPEED 10

void editPixel(SDL_Surface *testSurface, int x, int y);
void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile);
Uint32 pixelActive(SDL_Surface *testSurface, int x, int y);

int explosionPattern[3][12][6] =
{
{
{0, 1, 1, 1, 0, 0},
{0, 1, 1, 1, 0, 0},
{0, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 1, 1},
{0, 1, 1, 1, 1, 1},
{0, 1, 1, 1, 1, 0},
{0, 0, 1, 1, 0, 0},
{0, 0, 1, 0, 0, 0}
},
{
{0, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 0, 0},
{0, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 0, 0},
{0, 0, 1, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0}
},
{
{0, 0, 1, 1, 0, 0},
{0, 0, 1, 1, 0, 0},
{0, 1, 1, 1, 1, 1},
{0, 1, 1, 1, 1, 0},
{1, 1, 1, 1, 0, 0},
{1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 0},
{0, 1, 1, 1, 0, 0},
{0, 0, 1, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0}
}};


int main()
{

  SDL_Rect shield;
  shield.w = 88;
  shield.h = 64;
  shield.y = HEIGHT/2;

  SDL_Rect shields[4] = {shield, shield, shield, shield};

  for(int i = 0; i < 4; ++i)
  {
    shields[i].x = (WIDTH-(shield.w*4))/8 + i*(shield.w+(WIDTH-(shield.w*4))/4);
  }

  SDL_Rect player;
  player.w = 30;
  player.h = 20;
  player.x = (WIDTH-player.w)/2;
  player.y = HEIGHT-player.h;

  const Uint8 *keystate;

  SDL_Rect projectile;
  projectile.x = 0;
  projectile.y = 0;
  projectile.w = 2;
  projectile.h = 8;

  int projectileActive = 0;

  int colX = 0;
  int colY = 0;


  // initialise SDL and check that it worked otherwise exit
  // see here for details http://wiki.libsdl.org/CategoryAPI
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }
  // we are now going to create an SDL window

  SDL_Window *win = 0;
  win = SDL_CreateWindow("Invaders", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (win == 0)
  {
    printf("%s\n",SDL_GetError());
    return EXIT_FAILURE;
  }
  // the renderer is the core element we need to draw, each call to SDL for drawing will need the
  // renderer pointer
  SDL_Renderer *ren = 0;
  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  // check to see if this worked
  if (ren == 0)
  {
    printf("%s\n",SDL_GetError() );
    return EXIT_FAILURE;
  }
  // this will set the background colour to white.
  // however we will overdraw all of this so only for reference
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);

  SDL_Surface *testSurface[4];
  SDL_Texture *textureFromSurface[4];

  for(int i = 0; i < 4; ++i)
  {
    testSurface[i] = IMG_Load("shieldTexture.png");
    if(!testSurface[i])
    {
     printf("IMG_Load: %s\n", IMG_GetError());
     return EXIT_FAILURE;
    }
    textureFromSurface[i] = SDL_CreateTextureFromSurface(ren, testSurface[i]);
  }

  keystate = SDL_GetKeyboardState(NULL);

  int quit=0;
  // now we are going to loop forever, process the keys then draw

  printf("%i : %i\n", explosionPattern[0][0][2], explosionPattern[1][0][2]);

  while (quit !=1)
  {
    SDL_Event event;
    // grab the SDL even (this will be keys etc)
    while (SDL_PollEvent(&event))
    {
      // look for the x of the window being clicked and exit
      if (event.type == SDL_QUIT)
        quit = 1;
      // check for a key down
      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym)
        {
        // if we have an escape quit
        case SDLK_ESCAPE : quit=1; break;
        case SDLK_SPACE :
        {
          if(!projectileActive)
          {
            projectileActive = 1;
            projectile.x = player.x+player.w/2;
            projectile.y = player.y;
          }
          break;
        }

       }
      }
    }

  if(keystate[SDL_SCANCODE_LEFT])
    player.x -= 4;
  if(keystate[SDL_SCANCODE_RIGHT])
    player.x += 4;


  // now we clear the screen (will use the clear colour set previously)
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);

  for(int i = 0; i < 4; ++i)
    SDL_RenderCopy(ren, textureFromSurface[i], NULL, &shields[i]);

  if(projectileActive)
  {
    shootPewPew(ren, &projectile);
    for(int i = 0; i < 4; ++i)
    {
      if(SDL_HasIntersection(&projectile, &shields[i]))
      {
        colX = (projectile.x - shields[i].x) / (shields[i].w/testSurface[i]->w);
        colY = (projectile.y+PROJECTILESPEED-1 - shields[i].y) / (shields[i].h/testSurface[i]->h);
        if(pixelActive(testSurface[i], colX, colY) == 0x0000FF00)
        {
          projectileActive = 0;
          editPixel(testSurface[i], colX, colY);
          textureFromSurface[i] = SDL_CreateTextureFromSurface(ren, testSurface[i]);
        }
      }
      if(projectile.y < 0)
        projectileActive = 0;
    }
  }

  //SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, &player);
  // Up until now everything was drawn behind the scenes.
  // This will show the new, red contents of the window.
  SDL_RenderPresent(ren);

  }

  SDL_Quit();
  return 0;
}

void editPixel(SDL_Surface *testSurface, int x, int y)
{
  Uint8 *index;
  Uint32 *colour;
  int randomPattern = rand()%3;
  index = (Uint8 *)testSurface->pixels;

  for(int r = 0; r < 12; ++r)
  {
    if((y-r) >= 0)
    {
      for(int c = 0; c < 6; ++c)
      {
        if((x+(c-3)) >= 0 && (x+(c-3)) < testSurface->w)
        {
          if(explosionPattern[randomPattern][r][c])
          {
            colour = (Uint32 *)&index[(testSurface->pitch*(y-r) + testSurface->format->BytesPerPixel*(x+(c-3)))];
            *colour = 0x00000000;
          }
        }
        else
          continue;
      }
    }
    else
    {
      break;
    }
  }
}

Uint32 pixelActive(SDL_Surface *testSurface, int x, int y)
{
  Uint8 *index;
  Uint32 *value;
  index = (Uint8 *)testSurface->pixels;
  value = (Uint32 *)&index[(testSurface->pitch*y + testSurface->format->BytesPerPixel*x)];
  return *value;
}

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile)
{
  projectile->y -= PROJECTILESPEED;

  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, projectile);
}
