#include <lh/cast/static.h>
#include <lh/float/sqrt.h>
#include <lh/math/isqrt.h>

lh_int_t
lh_math_isqrt(lh_sllong_t value)
{
    lh_sllong_t root;
    if (value <= 0)
    {
        return 0;
    }
    /* The float root lands within one of the right answer at any scale a screen
       asks about, and the two steps below make it exact whatever the float
       said: the first walks the root down while its square is too big, the
       second walks it up while one past it still fits. */
    root = lh_cast_static(lh_sllong_t, lh_float_sqrt(lh_cast_static(lh_float_t, value)));
    while (root > 0 && root * root > value)
    {
        root -= 1;
    }
    while ((root + 1) * (root + 1) <= value)
    {
        root += 1;
    }
    return lh_cast_static(lh_int_t, root);
}
