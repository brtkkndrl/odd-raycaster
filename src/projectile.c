#include "projectile.h"

Sprite Projectile_getSprite(Projectile *e)
{
    Sprite s;
    s.pos = e->pos;
    s.tx = 0;
    s.ty = 32 + 1;
    s.tw = 32;
    s.th = 32;
    return s;
}

Projectile Projectile_create(Vector2f pos, Vector2f dir, float radius){
    Projectile p;
    p.pos = pos;
    p.circle.p.x = p.pos.x;
    p.circle.p.y = p.pos.y;
    p.circle.r = 0.1f;
    p.dir = dir;
    p.lifetime = 0.0f;
    return p;
}