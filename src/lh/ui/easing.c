/**
 * @file easing.c
 * @brief Implementation of `lh/ui/easing.h`.
 */

#include <lh/ui/easing.h>
#include <lh/cast/static.h>

/** Each curve at time 0/16 .. 16/16, in weights of 255: the Bézier's y where its x is
   that time, solved once off the target and kept as numbers. */
static const lh_u8_t lh_ui_easing_tables[5][17] = {
    {0, 12, 35, 68, 104, 137, 165, 187, 205, 219, 230, 238, 245, 250, 253, 254, 255}, /* ease */
    {0, 16, 32, 48, 64, 80, 96, 112, 128, 143, 159, 175, 191, 207, 223, 239, 255},    /* linear */
    {0, 2, 7, 14, 24, 36, 49, 64, 80, 98, 117, 137, 159, 181, 204, 229, 255},         /* ease-in */
    {0, 26, 51, 74, 96, 118, 138, 157, 175, 191, 206, 219, 231, 241, 248, 253, 255},  /* ease-out */
    {0, 2, 8, 18, 33, 52, 75, 100, 128, 155, 180, 203, 222, 237, 247, 253, 255}, /* ease-in-out */
};

lh_u32_t
lh_ui_easing_apply(lh_ui_easing_t easing, lh_u32_t time)
{

    if (time >= 255U)
    {
        return 255U;
    }
    if (easing == lh_ui_easing_linear)
    {
        return time;
    }
    const lh_u8_t *table;
    table = lh_ui_easing_tables[lh_cast_static(unsigned, easing) <= 4U
                                    ? lh_cast_static(unsigned, easing)
                                    : 0U];
    /* Which sixteenth the time is in, and how far into it -- then straight between
       the two samples either side. */
    lh_u32_t at = time * 16U / 255U;
    lh_u32_t part = time * 16U - at * 255U;
    return (table[at] * (255U - part) + table[at + 1U] * part) / 255U;
}
