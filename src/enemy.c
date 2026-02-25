#include "enemy.h"

Enemy Enemy_create(Vector2f pos){
    Enemy e;
    e.pos = pos;
    e.circle.p.x = 4.5f;
    e.circle.p.y = 4.5f;
    e.circle.r = ENEMY_WIDTH * 0.5f;
    e.width = ENEMY_WIDTH;
    e.attackTimeCounter = 0.0f;
    e.hitAnimTimeCounter = 0.0f;
    e.isHitAnimPlaying = false;
    return e;
}

Sprite Enemy_getSprite(Enemy *e)
{
    Sprite s;
    s.pos = e->pos;
    s.tx = e->isHitAnimPlaying ? 32 + 1 : 0;
    s.ty = 0;
    s.tw = 32;
    s.th = 32;
    return s;
}

void Enemy_takeDamage(Enemy *e)
{
    e->attackTimeCounter = 0.0f;
    e->hitAnimTimeCounter = 0.0f;
    e->isHitAnimPlaying = true;
}