#include <lh/vec4.h>
#include <lh/cast/static.h>
#include <lh/float/sqrt.h>

lh_vec4_t
lh_vec4_make(lh_float_t x, lh_float_t y, lh_float_t z, lh_float_t w)
{
    lh_vec4_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

lh_vec4_t
lh_vec4_add(lh_vec4_t a, lh_vec4_t b)
{
    return lh_vec4_make(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

lh_vec4_t
lh_vec4_sub(lh_vec4_t a, lh_vec4_t b)
{
    return lh_vec4_make(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

lh_vec4_t
lh_vec4_scale(lh_vec4_t v, lh_float_t s)
{
    return lh_vec4_make(v.x * s, v.y * s, v.z * s, v.w * s);
}

lh_vec4_t
lh_vec4_neg(lh_vec4_t v)
{
    return lh_vec4_make(-v.x, -v.y, -v.z, -v.w);
}

lh_float_t
lh_vec4_dot(lh_vec4_t a, lh_vec4_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

lh_float_t
lh_vec4_length_sq(lh_vec4_t v)
{
    return lh_vec4_dot(v, v);
}

lh_float_t
lh_vec4_length(lh_vec4_t v)
{
    return lh_float_sqrt(lh_vec4_length_sq(v));
}

lh_vec4_t
lh_vec4_normalize(lh_vec4_t v)
{
    const lh_float_t length = lh_vec4_length(v);
    if (length == 0.0f)
    {
        return v;
    }
    return lh_vec4_scale(v, 1.0f / length);
}

lh_vec4_t
lh_vec4_lerp(lh_vec4_t a, lh_vec4_t b, lh_float_t t)
{
    return lh_vec4_make(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
}

lh_bool_t
lh_vec4_near(lh_vec4_t a, lh_vec4_t b, lh_float_t eps)
{
    const lh_vec4_t d = lh_vec4_sub(a, b);
    return lh_cast_static(lh_bool_t, (d.x <= eps && -d.x <= eps) &&
                       (d.y <= eps && -d.y <= eps) &&
                       (d.z <= eps && -d.z <= eps) &&
                       (d.w <= eps && -d.w <= eps));
}
