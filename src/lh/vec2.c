#include <lh/vec2.h>
#include <lh/cast/static.h>
#include <lh/float/sqrt.h>

lh_vec2_t
lh_vec2_make(lh_float_t x, lh_float_t y)
{
    lh_vec2_t v;
    v.x = x;
    v.y = y;
    return v;
}

lh_vec2_t
lh_vec2_add(lh_vec2_t a, lh_vec2_t b)
{
    return lh_vec2_make(a.x + b.x, a.y + b.y);
}

lh_vec2_t
lh_vec2_sub(lh_vec2_t a, lh_vec2_t b)
{
    return lh_vec2_make(a.x - b.x, a.y - b.y);
}

lh_vec2_t
lh_vec2_scale(lh_vec2_t v, lh_float_t s)
{
    return lh_vec2_make(v.x * s, v.y * s);
}

lh_vec2_t
lh_vec2_neg(lh_vec2_t v)
{
    return lh_vec2_make(-v.x, -v.y);
}

lh_float_t
lh_vec2_dot(lh_vec2_t a, lh_vec2_t b)
{
    return a.x * b.x + a.y * b.y;
}

lh_float_t
lh_vec2_length_sq(lh_vec2_t v)
{
    return lh_vec2_dot(v, v);
}

lh_float_t
lh_vec2_length(lh_vec2_t v)
{
    return lh_float_sqrt(lh_vec2_length_sq(v));
}

lh_vec2_t
lh_vec2_normalize(lh_vec2_t v)
{
    const lh_float_t length = lh_vec2_length(v);
    if (length == 0.0f)
    {
        return v;
    }
    return lh_vec2_scale(v, 1.0f / length);
}

lh_vec2_t
lh_vec2_lerp(lh_vec2_t a, lh_vec2_t b, lh_float_t t)
{
    return lh_vec2_make(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}

lh_bool_t
lh_vec2_near(lh_vec2_t a, lh_vec2_t b, lh_float_t eps)
{
    const lh_vec2_t d = lh_vec2_sub(a, b);
    return lh_cast_static(lh_bool_t, (d.x <= eps && -d.x <= eps) &&
                       (d.y <= eps && -d.y <= eps));
}
