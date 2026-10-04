# Scene

How to put a widget on a screen, how to hang a paint effect on it, and how
to write either one yourself when the library does not have the one you
want.

A new kind does not go into lh. It is a class in your program. You include
the headers, define the class, and create instances with
`lh_entity_create`. Nothing is registered.

Headers for what ships: `lh/entity.h`, `lh/entity/2d.h`, `lh/entity/screen.h`,
`lh/entity/label.h`, `lh/entity/button.h`, `lh/entity/circle.h`,
`lh/ui/style.h`, `lh/ui/theme.h`, `lh/ui/effect.h`, `lh/ui/shadow.h`,
`lh/ui/blur.h`, `lh/ui/glass.h`.

---

## 1. The model

Everything on a screen is an entity. An entity has a class. The class says
how big the object is and which function answers its events. A derived class
names its base and starts with the base's fields, so a pointer to the
derived object is also a pointer to the base.

A 2D entity adds a place, a box, a style and an effect:

| Piece | What it is | Who owns it |
|---|---|---|
| Class | Constant description of one kind. One per kind. | The program that defines it. |
| Entity | One instance in the tree. | Its parent. Deleting the parent deletes it. |
| Style | Background color and text color. | You. The entity only points at it. |
| Effect | One paint pass, or a chain of them. | You. The entity only points at it. |

A style and an effect must outlive every entity that points at them.
Changing either does not redraw by itself. Move, resize, or call
`lh_entity_invalidate` on something that covers the area.

Drawing one entity goes in this order:

1. Its effect, if it has one. A shadow is painted here, under the fill.
   Glass blurs whatever is already in the box.
2. The style's rectangle, unless the entity has
   `lh_entity_flags_own_background`.
3. `LH_ENTITY_EVENT_DRAW`, for anything the kind paints itself.
4. Its children. A box cuts them to itself unless it has
   `lh_entity_flags_overflow_visible`.

`LH_ENTITY_EVENT_USER` (`0x1000`) and `lh_entity_flags_user` are the first
values free for your own events and flags.

---

## 2. Use a button that ships

`lh_entity_button_class` is a 2D entity. The box is the hit area. The style
paints it. The words are a label you put inside it, not a string stored on
the button. Give that label `lh_entity_flags_event_bubble`, or a press on
the words never reaches the button.

A press followed by a release over the same button sends
`LH_ENTITY_EVENT_CLICKED`. Listen with `lh_entity_add_handler`. The handler
is called after the class, and it is freed with the entity.

```c
void
on_button(lh_entity_event_t *event, lh_ptr user)
{
    (void)user;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED)
    {
        return;
    }
    /* the button was clicked */
}

lh_entity_t *button = lh_entity_create(&lh_entity_button_class, screen);
lh_entity_2d_set_position((lh_entity_2d_t *)button, lh_math_vec2_make(32.0f, 32.0f));
lh_entity_2d_set_size((lh_entity_2d_t *)button, lh_math_vec2_make(148.0f, 40.0f));
lh_entity_2d_set_style((lh_entity_2d_t *)button, &button_style);
lh_entity_button_set_pressed_style((lh_entity_button_t *)button, &pressed_style);
lh_entity_add_handler(button, on_button, lh_null);
```

`button_style` and `pressed_style` are `lh_ui_style_t` values you keep. Set
their colors with `lh_ui_style_set_bg_color` and
`lh_ui_style_set_text_color`. Alpha 0 paints nothing.

The same shape covers the other kinds that ship:

| Class | Header | What you get |
|---|---|---|
| `lh_entity_screen_class` | `lh/entity/screen.h` | The root. Create it with `lh_entity_create_root`. |
| `lh_entity_2d_class` | `lh/entity/2d.h` | A plain box. A parent, or a hit area. |
| `lh_entity_label_class` | `lh/entity/label.h` | One line of text. The string and the font are not copied. |
| `lh_entity_button_class` | `lh/entity/button.h` | The click above. |
| `lh_entity_circle_class` | `lh/entity/circle.h` | A disc inscribed in the box. The edge is covered per pixel. |

`lh_ui_theme_apply` points a screen, a label or a button at the theme's
styles. Any other class is left alone. Set its style yourself.

---

## 3. Use an effect that ships

An effect is a record plus a function that paints it. The entity holds one
pointer. Null paints nothing, so a box with no effect does not pay for one.

| Effect | Header | What it paints |
|---|---|---|
| `lh_ui_shadow_t` | `lh/ui/shadow.h` | A soft shadow around the box. |
| `lh_ui_blur_t` | `lh/ui/blur.h` | The pixels already in the box, averaged. |
| `lh_ui_glass_t` | `lh/ui/glass.h` | That blur, then a tint on top. |

```c
lh_ui_shadow_t shadow;

lh_ui_shadow_init(&shadow);
lh_ui_shadow_set_color(&shadow, lh_ui_color_make(0, 0, 0, 80));
lh_ui_shadow_set_spread(&shadow, 16);          /* fade distance, pixels */
lh_ui_shadow_set_offset(&shadow, 0, 5);        /* positive y moves it down */
lh_ui_shadow_set_sides(&shadow, LH_UI_SHADOW_SIDE_BOTTOM);
lh_entity_2d_set_effect((lh_entity_2d_t *)button, lh_ui_shadow_effect(&shadow));
```

`spread` of 0 paints nothing. `LH_UI_SHADOW_SIDE_LEFT`, `_RIGHT`, `_TOP` and
`_BOTTOM` combine. `LH_UI_SHADOW_SIDE_ALL` is every edge.

`lh_ui_effect_set_next` draws a second effect after the first. The head of
the chain is what you pass to `lh_entity_2d_set_effect`.

A window can wear the same shadow. The record is the one above.
`lh_os_window_set_shadow` does not copy it. Call it again after you change
the spread or the offset, so the window's padding still fits the shadow.
Null takes the shadow off. The scene's pixels stay the card. The shadow is
the frame around them.

---

## 4. Write an effect

A new effect is a struct that starts with `lh_ui_effect_fields`, then your
own fields. Two functions: `draw` paints into the canvas, `outset` says how
many pixels past the box you touch (0 if you stay inside). A class object
points at those functions. `lh_ui_effect_init` stores the class on the
record.

`draw` receives the box in screen pixels and the box's corner radius.
Zero radius is a square. Stay inside the canvas clip. `lh_ui_canvas_fill_rect`
and `lh_ui_canvas_blend_pixel` already do.

This tint is a whole effect. It is not part of lh. It lives in the program
that wants it.

```c
struct my_tint
{
    lh_ui_effect_fields(const lh_ui_effect_class_t *, const lh_ui_effect_t *);
    lh_ui_color_t color;
};

void
my_tint_draw(const lh_ui_effect_t *self, lh_ui_canvas_t *canvas, lh_math_rect_t box,
             lh_int_t corner)
{
    const struct my_tint *const tint = (const struct my_tint *)self;
    (void)corner;
    lh_ui_canvas_fill_rect(canvas, box, tint->color);
}

lh_int_t
my_tint_outset(const lh_ui_effect_t *self)
{
    (void)self;
    return 0;
}

const lh_ui_effect_class_t my_tint_class = {my_tint_draw, my_tint_outset};

void
my_tint_init(struct my_tint *self, lh_ui_color_t color)
{
    lh_ui_effect_init((lh_ui_effect_t *)self, &my_tint_class);
    self->color = color;
}
```

Attach it the same way as a shadow:

```c
struct my_tint tint;
my_tint_init(&tint, lh_ui_color_make(255, 255, 255, 40));
lh_entity_2d_set_effect(box, (const lh_ui_effect_t *)&tint);
```

If the paint extends past the box, `outset` returns that margin. The screen
uses it when it marks the dirty area, so a later redraw still covers the
overflow.

Shadow, blur and glass in `src/lh/ui/` are this pattern and nothing more.
Read `shadow.c` for a paint that steps outside the box, and `glass.c` for
one that reads the pixels already there.

---

## 5. Write a widget

A new widget is a struct that starts with `lh_entity_fields` and
`lh_entity_2d_fields`, then your own fields. The class names
`lh_entity_2d_class` as its base and gives the size of your struct. The
event function is how the kind behaves. Pass null for a constructor or a
destructor you do not need. The memory is zeroed before the constructor
runs.

The pad below is a box that sends `LH_ENTITY_EVENT_CLICKED` when a press
and a release land on it. The shipped button is this, plus a style it
swaps while the pointer is down. Use the button when that is what you
want. Write a class when it is not.

```c
struct my_pad
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *,
                        const lh_ui_effect_t *);
    lh_bool_t down;
};

void
my_pad_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    struct my_pad *const pad = (struct my_pad *)self;
    const lh_uint_t code = lh_entity_event_get_code(event);

    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        pad->down = lh_bool_true;
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_UP && pad->down)
    {
        const lh_math_vec2_t *const point =
            (const lh_math_vec2_t *)lh_entity_event_get_param(event);
        pad->down = lh_bool_false;
        if (point != lh_null &&
            lh_entity_2d_contains((const lh_entity_2d_t *)self, *point))
        {
            lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED,
                                 lh_entity_event_get_param(event));
        }
    }
}

const lh_entity_class_t my_pad_class = lh_entity_class_initializer(
    &lh_entity_2d_class, sizeof(struct my_pad), lh_null, lh_null, my_pad_on_event);
```

Create it like any other entity:

```c
lh_entity_t *pad = lh_entity_create(&my_pad_class, screen);
lh_entity_2d_set_size((lh_entity_2d_t *)pad, lh_math_vec2_make(80.0f, 32.0f));
lh_entity_2d_set_style((lh_entity_2d_t *)pad, &pad_style);
lh_entity_add_handler(pad, on_button, lh_null);
```

Paint that the style cannot express goes in `LH_ENTITY_EVENT_DRAW`. The
parameter is the `lh_ui_canvas_t`. The clip is already set. Set
`lh_entity_flags_own_background` when you paint the whole box yourself and
do not want the style's rectangle under it. `circle.c` does this.

Read the pointer position from `LH_ENTITY_EVENT_POINTER_DOWN`,
`_POINTER_UP` and `_POINTER_MOVE`. The parameter is a
`const lh_math_vec2_t *` in screen pixels. `lh_entity_2d_contains` tests
the box.

`src/lh/entity/button.c` is the click with a pressed style.
`src/lh/entity/label.c` is a kind that paints in `DRAW`.
`src/lh/entity/circle.c` is a kind that skips the rectangular fill.

---

## 6. Widgets that ship

The ready ones share a few records, so a new control is usually one of
these with a different paint:

| What you want | Header | Shared with |
|---|---|---|
| Progress bar | `lh/entity/range.h` | The value every ranged widget starts with. |
| Trackbar | `lh/entity/slider.h` | That value, plus a drag. |
| Knob | `lh/entity/knob.h` | The same value, turned by the pointer. |
| Spin box | `lh/entity/spin.h` | The same value, stepped by the sides. |
| Scrollbar | `lh/entity/scroll.h` | The same value. The thumb style and the box size are the customization. |
| Scrollable page | `lh/entity/view.h` | A clip. The scrollbar's value is the offset. |
| Check, switch, toggle | `lh/entity/option.h` | One boolean. Three classes. |
| One-or-many group | `lh/entity/group.h` | Parent of those options. |
| List | `lh/entity/list.h` | The group's one-or-many mode, on rows. |
| Combo box, dropdown | `lh/entity/combo.h` | A list that opens under the box. |
| Tabs, pages | `lh/entity/tabs.h`, `lh/entity/pages.h` | An exclusive group of toggles, and one visible child. |
| Text field | `lh/entity/field.h` | One buffer. `set_lines(1)` is a single line. |
| On-screen keyboard | `lh/entity/keys.h` | Sends keys to the screen focus. |
| Picture | `lh/entity/image.h` | A child of a button: icon, words, or both. |
| Hold | `lh/entity/button.h` | `lh_entity_button_set_repeat`. |

A window opens where the OS puts it. `lh_os_window_set_place` with
`LH_OS_WINDOW_PLACE_CENTER` puts it in the middle of the primary monitor's
work area, which is the place a program asks for at startup.

---

## 7. Flex

`lh_entity_flex_set_on` makes any entity lay out its direct children the way
CSS `display: flex` does. The container's size is the flex line. A child with
a size keeps it. A child with size 0 on an axis, or a flex container you never
sized, is `auto` on that axis: it hugs its children, and `align-items: stretch`
may give it the line's cross size.

```c
lh_entity_flex_set_on(row, lh_bool_true);
lh_entity_flex_set_justify(row, LH_ENTITY_FLEX_SPACE_BETWEEN);
lh_entity_flex_set_align(row, LH_ENTITY_FLEX_CENTER);
lh_entity_flex_set_gap(row, 8);
lh_entity_flex_set_padding(row, 0, 12, 0, 12);
lh_entity_flex_item_set_grow(panel, 1);
```

Direction is row, row-reverse, column or column-reverse. Wrap is nowrap, wrap
or wrap-reverse. Justify is start, end, center, space-between, space-around
or space-evenly. Align-items and align-self add stretch. `row-gap` and
`column-gap` follow CSS: in a row, the column gap sits between items.

::lh_entity_screen_render lays the tree out, parents first, so a nested flex
receives the size its parent just assigned. Hidden children are left out.
Min and max sizes, item margins and baseline alignment are not part of this.


