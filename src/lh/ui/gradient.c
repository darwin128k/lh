/**
 * @file gradient.c
 * @brief Implementation of lh/ui/gradient.h and lh/ui/gradient/stop.h.
 */

#include <lh/assert/runtime.h>
#include <lh/byte.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/ui/gradient.h>
#include <lh/ui/gradient/stop/count.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

/* ── Stop ────────────────────────────────────────────────────────────────── */

lh_ui_gradient_stop_t
lh_ui_gradient_stop_make(lh_ui_color_t color, lh_byte_t frac)
{
    lh_ui_gradient_stop_t stop;
    lh_ui_gradient_stop_set_color(lh_addr_of(stop), color);
    lh_ui_gradient_stop_set_frac(lh_addr_of(stop), frac);
    return stop;
}

const lh_ui_color_t *
lh_ui_gradient_stop_get_color_as_const(const lh_ui_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_ui_color_t *
lh_ui_gradient_stop_get_color(lh_ui_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->color);
}

lh_byte_t
lh_ui_gradient_stop_get_frac(const lh_ui_gradient_stop_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frac;
}

lh_void
lh_ui_gradient_stop_set_color(lh_ui_gradient_stop_t *self, lh_ui_color_t color)
{
    lh_assert_runtime_ref(self);
    lh_ptr_deref(lh_ui_gradient_stop_get_color(self)) = color;
}

lh_void
lh_ui_gradient_stop_set_frac(lh_ui_gradient_stop_t *self, lh_byte_t frac)
{
    lh_assert_runtime_ref(self);
    self->frac = frac;
}

/* ── Gradient ────────────────────────────────────────────────────────────── */

lh_ui_gradient_t
lh_ui_gradient_make_empty(void)
{
    lh_ui_gradient_t gradient;
    lh_ui_gradient_init(lh_addr_of(gradient));
    return gradient;
}

lh_void
lh_ui_gradient_init(lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    self->stops_count = 0;
}

lh_void
lh_ui_gradient_init_stops(lh_ui_gradient_t *self, const lh_ui_color_t *colors, const lh_byte_t *fracs,
                          lh_ui_gradient_stop_count_t num_stops)
{
    lh_ui_gradient_stop_count_t i;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(colors);
    lh_assert_runtime_ifn(num_stops >= 1U, lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_ifn(num_stops <= LH_LIBRARY_OPTION_UI_GRADIENT_MAX_STOPS,
                          lh_runtime_error_code_invalid_argument);
    self->stops_count = num_stops;
    for (i = 0; i < num_stops; ++i)
    {
        lh_byte_t frac;
        if (lh_null_eq(fracs))
        {
            if (num_stops == 1U)
            {
                frac = 0;
            }
            else
            {
                frac = (lh_byte_t)((i * 255U) / (num_stops - 1U));
            }
        }
        else
        {
            frac = fracs[i];
        }
        self->stops[i] = lh_ui_gradient_stop_make(colors[i], frac);
    }
}

lh_ui_gradient_stop_count_t
lh_ui_gradient_get_stop_count(const lh_ui_gradient_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stops_count;
}

const lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop_as_const(const lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(index < self->stops_count, lh_runtime_error_code_invalid_argument);
    return lh_addr_of(self->stops[index]);
}

lh_ui_gradient_stop_t *
lh_ui_gradient_get_stop(lh_ui_gradient_t *self, lh_ui_gradient_stop_count_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(index < self->stops_count, lh_runtime_error_code_invalid_argument);
    return lh_addr_of(self->stops[index]);
}
