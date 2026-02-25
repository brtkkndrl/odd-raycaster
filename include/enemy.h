#ifndef ENEMY_H
#define ENEMY_H

#define ENEMY_WIDTH 0.75          // 24/32
#define ENEMY_HIT_ANIM_TIME 0.25f // how long hit anim plays
#define ENEMY_SHOOT_DELAY 1.25F   // delay between shots

#include <stdlib.h>
#include "cute_c2.h"
#include "vec2f.h"
#include "sprite.h"
#include <stdbool.h>

typedef struct Enemy
{
    Vector2f pos;
    c2Circle circle;
    float attackTimeCounter;
    float hitAnimTimeCounter;
    bool isHitAnimPlaying;
    float width;
} Enemy;

Enemy Enemy_create(Vector2f pos);

typedef struct EnemyArray
{
    int size;
    int capacity;
    Enemy *enemies;
} EnemyArray;

void EnemyArray_init(EnemyArray *ea, int s);

void EnemyArray_free(EnemyArray *ea);

void EnemyArray_add(EnemyArray *ea, Enemy e);

void Enemy_takeDamage(Enemy *e);

Sprite Enemy_getSprite(Enemy *e);

#endif