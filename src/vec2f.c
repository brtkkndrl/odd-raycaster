#include "vec2f.h"
#include <math.h>

Vector2f Vector2f_create(float x, float y){
    Vector2f v;
    v.x = x;
    v.y = y;
    return v;
}

Vector2f Vector2f_add(Vector2f a, Vector2f b)
{
    Vector2f c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    return c;
}

Vector2f Vector2f_sub(Vector2f a, Vector2f b)
{
    Vector2f c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    return c;
}

Vector2f Vector2f_multScalar(Vector2f a, float s)
{
    a.x *= s;
    a.y *= s;
    return a;
}

float Vector2f_mag(Vector2f v)
{
    return sqrt(v.x*v.x + v.y*v.y);
}

float Vector2f_dot(Vector2f a, Vector2f b){
    return a.x*b.x + a.y*b.y;
}

Vector2f Vector2f_norm(Vector2f v)
{
    float m = Vector2f_mag(v);
    if(m > 0)
    {
        v.x = v.x/m;
        v.y = v.y/m;
    }
    else
    {
        v.x = 0;
        v.y = 0;
    }
    return v;
}

Vector2f Vector2f_right(Vector2f v){
    Vector2f v2;
    v2.x = -v.y;
    v2.y = v.x;
    return v2;
}
