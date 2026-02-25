#ifndef VEC2F_H
#define VEC2F_H

typedef struct Vector2f
{
    float x, y;
} Vector2f;

Vector2f Vector2f_create(float x, float y);

Vector2f Vector2f_add(Vector2f, Vector2f);

Vector2f Vector2f_sub(Vector2f, Vector2f);

Vector2f Vector2f_multScalar(Vector2f, float);

float Vector2f_mag(Vector2f);

Vector2f Vector2f_norm(Vector2f);

Vector2f Vector2f_right(Vector2f);

float Vector2f_dot(Vector2f a, Vector2f b);

#endif // VEC2F_H
