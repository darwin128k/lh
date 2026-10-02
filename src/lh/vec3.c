#include <lh/vec3.h>
#include <lh/cast/static.h>
#include <lh/float/sqrt.h>

lh_vec3_t
lh_vec3_make(lh_float_t x, lh_float_t y, lh_float_t z)
{
    lh_vec3_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

lh_vec3_t
lh_vec3_add(lh_vec3_t a, lh_vec3_t b)
{
    return lh_vec3_make(a.x + b.x, a.y + b.y, a.z + b.z);
}

lh_vec3_t
lh_vec3_sub(lh_vec3_t a, lh_vec3_t b)
{
    return lh_vec3_make(a.x - b.x, a.y - b.y, a.z - b.z);
}

lh_vec3_t
lh_vec3_scale(lh_vec3_t v, lh_float_t s)
{
    return lh_vec3_make(v.x * s, v.y * s, v.z * s);
}

lh_vec3_t
lh_vec3_neg(lh_vec3_t v)
{
    return lh_vec3_make(-v.x, -v.y, -v.z);
}

lh_float_t
lh_vec3_dot(lh_vec3_t a, lh_vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

lh_float_t
lh_vec3_length_sq(lh_vec3_t v)
{
    return lh_vec3_dot(v, v);
}

lh_float_t
lh_vec3_length(lh_vec3_t v)
{
    return lh_float_sqrt(lh_vec3_length_sq(v));
}

lh_vec3_t
lh_vec3_normalize(lh_vec3_t v)
{
    const lh_float_t length = lh_vec3_length(v);
    if (length == 0.0f)
    {
        return v;
    }
    return lh_vec3_scale(v, 1.0f / length);
}

lh_vec3_t
lh_vec3_lerp(lh_vec3_t a, lh_vec3_t b, lh_float_t t)
{
    return lh_vec3_make(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);
}

lh_bool_t
lh_vec3_near(lh_vec3_t a, lh_vec3_t b, lh_float_t eps)
{
    const lh_vec3_t d = lh_vec3_sub(a, b);
    return lh_cast_static(lh_bool_t, (d.x <= eps && -d.x <= eps) &&
                       (d.y <= eps && -d.y <= eps) &&
                       (d.z <= eps && -d.z <= eps));
}

lh_vec3_t
lh_vec3_cross(lh_vec3_t a, lh_vec3_t b)
{
    return lh_vec3_make(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
