#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// include the map for the maze.
// the width of the screen taking into account the maze and block
#define WIDTH 800
// the height of the screen taking into account the maze and block
#define HEIGHT 600
// an enumeration for direction to move USE more enums!

void editPixels(SDL_Surface *testSurface, int x, int y, int r, int g, int b);
int getPixel(SDL_Surface *testSurface, int x, int y);
void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile);

int main()
{

  SDL_Rect box;
  box.w = 80;
  box.h = 80;
  box.x = WIDTH/2;
  box.y = HEIGHT/2;

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

  Uint8 *index;
  SDL_Surface *testSurface;
  SDL_Texture *textureFromSurface;
  testSurface = IMG_Load("foo.png");
  if(!testSurface)
  {
    printf("IMG_Load: %s\n", IMG_GetError());
    return EXIT_FAILURE;
  }

  textureFromSurface = SDL_CreateTextureFromSurface(ren, testSurface);
  index = (Uint8 *)testSurface->pixels;
  /*for(int i = 0; i < testSurface->pitch*testSurface->h; ++i)
  {
    printf("%i ", index[i]);
    if(i == testSurface->pitch)
      printf("\n");
  }*/
  printf("%i", testSurface->w);

  keystate = SDL_GetKeyboardState(NULL);

  int quit=0;
  // now we are going to loop forever, process the keys then draw

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

  SDL_RenderCopy(ren, textureFromSurface, NULL, &box);

  if(projectileActive)
  {
    shootPewPew(ren, &projectile);
    if(projectile.y < 0)
      projectileActive = 0;

    if(SDL_HasIntersection(&projectile, &box))
    {
      colX = (projectile.x - box.x) / (box.w/testSurface->w);
      colY = (projectile.y - box.y) / (box.h/testSurface->h);
      if(getPixel(testSurface, colX, colY) == 255)
      {
        int value = getPixel(testSurface, colX, colY);
        printf("%i - %i - %i", value, colX, colY);
        projectileActive = 0;
        editPixels(testSurface, colX, colY, 0, 0, 0);
        printf("OSU\n");
        textureFromSurface = SDL_CreateTextureFromSurface(ren, testSurface);
      }

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

void editPixels(SDL_Surface *testSurface, int x, int y, int r, int g, int b)
{
  Uint8 *index;
  index = (Uint8 *)testSurface->pixels;
  int explodesUp = 1;
  int explodesLeft = 1;
  int explodesRight = 1;
  int up = 1;
  int left = 1;
  int right = 1;

  index[(testSurface->pitch*y) + testSurface->format->BytesPerPixel*x] = r;
  index[(testSurface->pitch*y) + testSurface->format->BytesPerPixel*x+1] = g;
  index[(testSurface->pitch*y) + testSurface->format->BytesPerPixel*x+2] = b;

  /*for(int i = 0; i < 15; ++i)
  {
    while(index[(testSurface->pitch*randomY) + testSurface->format->BytesPerPixel*randomX] == 0)
    {
      randomX = rand()%8;
      randomY = rand()%8;
    }

    index[(testSurface->pitch*randomY) + testSurface->format->BytesPerPixel*randomX] = r;
    index[(testSurface->pitch*randomY) + testSurface->format->BytesPerPixel*randomX+1] = g;
    index[(testSurface->pitch*randomY) + testSurface->format->BytesPerPixel*randomX+2] = b;
  }*/

  while(explodesUp)
  {
    if(rand()%(8-up) != 0 && y >= up)
    {
      index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*x] = r;
      index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*x+1] = g;
      index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*x+2] = b;

      while(explodesLeft)
      {
        if(rand()%(5-left-up) != 0 && x >= left)
        {
          index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*(x-left)] = r;
          index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*(x-left)+1] = g;
          index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*(x-left)+2] = b;
          ++left;
        }
        else
          explodesLeft = 0;
      }
      explodesLeft = 1;
      left = 1;

      while(explodesRight)
      {
        if(rand()%(5-right-up) != 0 && testSurface->w > right+x)
        {
          index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*(x+right)] = r;
          index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*(x+right)+1] = g;
          index[(testSurface->pitch*(y-up)) + testSurface->format->BytesPerPixel*(x+right)+2] = b;
          ++left;
        }
        else
          explodesRight = 0;
      }
      explodesRight = 1;
      right = 1;

      ++up;
    }
    else
      explodesUp = 0;
  }

}

int getPixel(SDL_Surface *testSurface, int x, int y)
{
  Uint8 *pixel;
  pixel = (Uint8 *)testSurface->pixels;
  int value = pixel[(testSurface->pitch*y) + testSurface->format->BytesPerPixel*x+1];
  return value;
}

void shootPewPew(SDL_Renderer *ren, SDL_Rect *projectile)
{
  projectile->y -= 6;

  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderFillRect(ren, projectile);
}
