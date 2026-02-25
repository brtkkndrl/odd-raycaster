#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <stdlib.h>
#include "vec2f.h"
#include "cute_c2.h"
#include "sprite.h"

typedef struct Projectile
{
    Vector2f pos;
    c2Circle circle;
    Vector2f dir;
    float lifetime;
} Projectile;

Projectile Projectile_create(Vector2f pos, Vector2f dir, float radius);

Sprite Projectile_getSprite(Projectile *e);

#endif