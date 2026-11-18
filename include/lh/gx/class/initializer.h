/**
 * @file initializer.h
 * @brief Brace-enclosed initializer of ::lh_gx_class_t, for a class in flash.
 */

#ifndef LH_GX_CLASS_INITIALIZER_H
#define LH_GX_CLASS_INITIALIZER_H

#include <lh/gx/class/covers/fn.h>
#include <lh/gx/class/hit/fn.h>
#include <lh/gx/class/input/fn.h>
#include <lh/gx/class/paint/fn.h>
#include <lh/gx/class/report/fn.h>

/**
 * @def lh_gx_class_initializer(paint_fn, covers_fn, hit_fn, input_fn, report_fn)
 * @brief The slots of a class, in order, for a constant table.
 *
 * A class is shared by every entity of its kind and never changes, so it is
 * usually a constant: `const lh_gx_class_t my_card = lh_gx_class_initializer(
 * my_card_paint, my_card_covers, lh_null, lh_null, lh_null);`. Each argument is
 * a function of the slot's type, or ::lh_null for a slot the class does not
 * fill; it goes through ::lh_cast_static, so a function of the wrong shape is a
 * compile error in C++.
 *
 * @param paint_fn  ::lh_gx_class_paint_fn, or ::lh_null.
 * @param covers_fn ::lh_gx_class_covers_fn, or ::lh_null.
 * @param hit_fn    ::lh_gx_class_hit_fn, or ::lh_null.
 * @param input_fn  ::lh_gx_class_input_fn, or ::lh_null.
 * @param report_fn ::lh_gx_class_report_fn, or ::lh_null.
 */
#define lh_gx_class_initializer(paint_fn, covers_fn, hit_fn, input_fn, report_fn)                  \
    lh_initializer(lh_cast_static(lh_ptr_of(lh_gx_class_paint_fn), paint_fn),                      \
                   lh_cast_static(lh_ptr_of(lh_gx_class_covers_fn), covers_fn),                    \
                   lh_cast_static(lh_ptr_of(lh_gx_class_hit_fn), hit_fn),                          \
                   lh_cast_static(lh_ptr_of(lh_gx_class_input_fn), input_fn),                      \
                   lh_cast_static(lh_ptr_of(lh_gx_class_report_fn), report_fn))

#endif /* LH_GX_CLASS_INITIALIZER_H */
