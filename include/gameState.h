#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "global.h"
#include "draw.h"
#include "drawRaycast.h"
#include "SDL2/SDL.h"
#include "collision.h"
#include "projectile.h"
#include "enemy.h"

#include "SDL2/SDL_ttf.h"

typedef struct GameState
{
    Ray *rays;
    Player player;
    Map map;
    EnemyArray enemies;
    EntityArray enemyProjectiles;
    KeysArray keys;
    float depthBuffer[SCREEN_WIDTH];
    SDL_Surface *screenSurface;
    SDL_Surface *deathScreenSurface;
    SDL_Texture *barTexture;
    SDL_Texture *frameTexture;
    SDL_Texture *gunTexture;
    SDL_Texture *soldierTexture;
    SDL_Texture *wallsTexture;
    SDL_Texture *vigneteTexture;
    SDL_Surface *wallsSurface;
    SDL_Surface *soldierSurface;
    TextureXAspectRatio youDiedText;
    uint32_t rndval;
    bool isDeathAnimFinished;
    bool isQuitRequested;
    TTF_Font *font;
    bool isPaused;
    IntInRange selectedOptionPaused;
} GameState;


void GameState_init(GameState *gameState, SDL_Renderer *renderer, TTF_Font *font);
void GameState_free(GameState *gameState);
void GameState_event(GameState *gameState, SDL_Event event);
void GameState_update(GameState *gameState, float delta);
void GameState_updateEnemies(EnemyArray *enemies, EntityArray *enemy_projectiles, Player *player, float delta);
void GameState_playerAttackEnemiesRaycast(EnemyArray *enemies, Player *player, Map *map, float maxDistance);
void GameState_updateProjectiles(EntityArray *projectiles, float delta);
void GameState_draw(GameState *gameState, SDL_Renderer *renderer);
void GameState_playerAttackEnemies(EnemyArray *enemies, Player *player, Map *map);

#endif
