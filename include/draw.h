#ifndef DRAW_H
#define DRAW_H

#define SQUARE_ROOT_OF_TWO 1.4142

#include "SDL2/SDL.h"
#include "player.h"
#include "rays.h"
#include "enemy.h"
#include "entityArray.h"
#include "global.h"
#include "utils.h"
#include "sprite.h"
#include "projectile.h"
#include "map.h"
#include "SDL2/SDL_ttf.h"

typedef struct DrawFloorThreadData
{
    SDL_Surface *screenSurface;
    Player *player;
    SDL_Surface *wallsSurface;
    int startY;
    int endY;
} DrawFloorThreadData;

typedef struct DrawWallsThreadData
{
    SDL_Surface *screenSurface;
    Player *player;
    Ray *rays;
    float *zBuffer;
    SDL_Surface *wallsSurface;
    int startX, endX;
} DrawWallsThreadData;

typedef struct ScalingData
{
    unsigned scalingFactor;
    unsigned offsetX;
    unsigned offsetY;
    unsigned rendererWidth;
    unsigned rendererHeight;
} ScalingData;

typedef struct TextureXAspectRatio
{
    SDL_Texture *texture;
    float aspectRatio;
} TextureXAspectRatio;

void TextureXAspectRation_free(TextureXAspectRatio *tt);
TextureXAspectRatio TextureXAspectRation_create(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color);

ScalingData ScalingData_create(SDL_Renderer *renderer);
void *drawFloor_threaded(void *vargp);
void *drawWalls_threaded(void *vargp);
void drawText(SDL_Renderer *renderer, SDL_Texture *texture, int sx, int sy, char *text);
void drawMap(SDL_Renderer *renderer, Player *player, EntityArray *enemies, Ray *rays, int iRayCount, Map *map);
void drawWeapon(SDL_Renderer *renderer, Player *player, SDL_Texture *gun_texture, ScalingData *scalingInfo);
void drawBar(SDL_Renderer *renderer, Player *player, SDL_Texture *bar_texture, ScalingData *scalingInfo);
void drawPausedBar(SDL_Renderer *renderer, TTF_Font *font, ScalingData *scalingInfo, int selectedOption);

#endif
