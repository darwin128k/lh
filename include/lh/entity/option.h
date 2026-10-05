/**
 * @file option.h
 * @brief A check, a switch, or a toggle. One record, three classes.
 *
 * All three flip a boolean and send ::LH_ENTITY_EVENT_CLICKED. The mark is
 * the text color and the box is the style background. A check keeps a
 * rounded frame; when it is on, a smaller rounded square sits inside.
 * Inside a
 * ::lh_entity_group_t the group's mode decides whether the siblings turn
 * off. The words, when they are wanted, are a label placed beside it.
 */

#ifndef LH_ENTITY_OPTION_H
#define LH_ENTITY_OPTION_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>
#include <lh/ui/style.h>

struct lh_entity_circle;

/**
 * @def LH_ENTITY_OPTION_CHECK
 * @brief A box with a mark.
 */
#define LH_ENTITY_OPTION_CHECK 0

/**
 * @def LH_ENTITY_OPTION_SWITCH
 * @brief A track with a circle. The rim around that circle is half the
 *        circle, so it stays inside the track.
 */
#define LH_ENTITY_OPTION_SWITCH 1

/**
 * @def LH_ENTITY_OPTION_TOGGLE
 * @brief A button that stays down.
 */
#define LH_ENTITY_OPTION_TOGGLE 2

/**
 * @struct lh_entity_option
 * @brief One boolean control.
 */
struct lh_entity_option
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    lh_int_t kind;
    lh_bool_t on;
    struct lh_entity_circle *thumb;
    lh_ui_style_t thumb_style;
};
typedef struct lh_entity_option lh_entity_option_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_option_t, the base of all three kinds.
 *
 * It has no instance of its own: it is what makes a check, a switch and a
 * toggle one kind of thing to a parent that has to decide between them. Before
 * this class each kind answered to nothing in common, so a parent asking "is
 * this child one of mine" had to try all three classes in turn.
 */
extern const lh_entity_class_t lh_entity_option_class;

/**
 * @brief A checkbox (::LH_ENTITY_OPTION_CHECK), derived from
 *        ::lh_entity_option_class.
 */
extern const lh_entity_class_t lh_entity_check_class;

/**
 * @brief A switch (::LH_ENTITY_OPTION_SWITCH), derived from
 *        ::lh_entity_option_class.
 */
extern const lh_entity_class_t lh_entity_switch_class;

/**
 * @brief A toggle button (::LH_ENTITY_OPTION_TOGGLE), derived from
 *        ::lh_entity_option_class.
 */
extern const lh_entity_class_t lh_entity_toggle_class;

/**
 * @brief True when @p self is on.
 */
lh_bool_t
lh_entity_option_is_on(const lh_entity_option_t *self);

/**
 * @brief Turn @p self on or off. A group in single mode turns the others off.
 */
lh_void
lh_entity_option_set_on(lh_entity_option_t *self, lh_bool_t on);

/**
 * @brief Set the flag and redraw, without asking the group anything.
 *
 * This is what a group calls on the members it is turning off. Through
 * ::lh_entity_option_set_on each of them would ask the group in turn, and
 * turning the first one off would take the rest with it.
 */
lh_void
lh_entity_option_set_on_raw(lh_entity_option_t *self, lh_bool_t on);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_OPTION_H */
