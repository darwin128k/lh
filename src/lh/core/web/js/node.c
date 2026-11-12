/**
 * @file node.c
 * @brief Implementation of `lh/core/web/js/node.h`.
 */

#include <lh/core/web/js/node.h>
#include <lh/core/assert.h>
#include <lh/core/runtime/allocator.h>

/* ── accessors ───────────────────────────────────────────────────────────── */

lh_list_node_t *
lh_web_js_node_get_link_mut(lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_list_node_t *
lh_web_js_node_get_link(const lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_web_js_node_t *
lh_web_js_node_get_by_link_mut(lh_list_node_t *link)
{
    return lh_list_entry(lh_web_js_node_t, link, link);
}

const lh_web_js_node_t *
lh_web_js_node_get_by_link(const lh_list_node_t *link)
{
    return lh_list_entry(const lh_web_js_node_t, link, link);
}

lh_web_js_kind_t
lh_web_js_node_get_kind(const lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_web_js_node_set_kind(lh_web_js_node_t *self, lh_web_js_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

lh_web_js_op_t
lh_web_js_node_get_op(const lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->op;
}

lh_void
lh_web_js_node_set_op(lh_web_js_node_t *self, lh_web_js_op_t op)
{
    lh_assert_runtime_ref(self);
    self->op = op;
}

lh_bool_t
lh_web_js_node_get_flag(const lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flag;
}

lh_void
lh_web_js_node_set_flag(lh_web_js_node_t *self, lh_bool_t flag)
{
    lh_assert_runtime_ref(self);
    self->flag = flag;
}

lh_str_t *
lh_web_js_node_get_name_mut(lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

const lh_str_t *
lh_web_js_node_get_name(const lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_str_t *
lh_web_js_node_get_text_mut(lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

const lh_str_t *
lh_web_js_node_get_text(const lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->text);
}

lh_list_t *
lh_web_js_node_get_children_mut(lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

const lh_list_t *
lh_web_js_node_get_children(const lh_web_js_node_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->children);
}

lh_bool_t
lh_web_js_node_has_kind(const lh_web_js_node_t *self, lh_web_js_kind_t kind)
{
    return lh_cast_static(lh_bool_t, lh_math_eq(lh_web_js_node_get_kind(self), kind));
}

lh_str_view_t
lh_web_js_node_get_name_view(const lh_web_js_node_t *self)
{
    lh_str_view_t view;

    lh_str_as_view(lh_web_js_node_get_name(self), lh_addr_of(view));
    return view;
}

lh_str_view_t
lh_web_js_node_get_text_view(const lh_web_js_node_t *self)
{
    lh_str_view_t view;

    lh_str_as_view(lh_web_js_node_get_text(self), lh_addr_of(view));
    return view;
}

/* ── init / deinit ───────────────────────────────────────────────────────── */

lh_void
lh_web_js_node_init(lh_web_js_node_t *self)
{
    lh_list_node_init(lh_web_js_node_get_link_mut(self));
    lh_web_js_node_set_kind(self, lh_web_js_kind_none);
    lh_web_js_node_set_op(self, lh_web_js_op_none);
    lh_web_js_node_set_flag(self, lh_bool_false);
    lh_str_init(lh_web_js_node_get_name_mut(self));
    lh_str_init(lh_web_js_node_get_text_mut(self));
    lh_list_init(lh_web_js_node_get_children_mut(self));
}

/* ── children and lifetime ───────────────────────────────────────────────── */

lh_web_js_node_t *
lh_web_js_node_push_back(lh_web_js_node_t *self, lh_web_js_node_t *child)
{
    lh_list_push_back(lh_web_js_node_get_children_mut(self), lh_web_js_node_get_link_mut(child));
    return self;
}

const lh_web_js_node_t *
lh_web_js_node_get_first(const lh_web_js_node_t *self)
{
    return lh_web_js_node_get_by_link(lh_list_get_first(lh_web_js_node_get_children(self)));
}

const lh_web_js_node_t *
lh_web_js_node_get_next(const lh_web_js_node_t *self, const lh_web_js_node_t *child)
{
    return lh_web_js_node_get_by_link(
        lh_list_get_next(lh_web_js_node_get_children(self), lh_web_js_node_get_link(child)));
}

lh_web_js_node_t *
lh_web_js_node_get_last_mut(lh_web_js_node_t *self)
{
    return lh_web_js_node_get_by_link_mut(lh_list_get_last(lh_web_js_node_get_children(self)));
}

const lh_web_js_node_t *
lh_web_js_node_get_last(const lh_web_js_node_t *self)
{
    return lh_web_js_node_get_by_link(lh_list_get_last(lh_web_js_node_get_children(self)));
}

lh_usize_t
lh_web_js_node_get_size(const lh_web_js_node_t *self)
{
    return lh_list_get_size(lh_web_js_node_get_children(self));
}

lh_void
lh_web_js_node_deinit(lh_web_js_node_t *self)
{
    lh_web_js_node_clear(self);
    lh_str_deinit(lh_web_js_node_get_name_mut(self));
    lh_str_deinit(lh_web_js_node_get_text_mut(self));
}

lh_void
lh_web_js_node_destroy(lh_web_js_node_t *self)
{
    lh_list_node_unlink(lh_web_js_node_get_link_mut(self));
    lh_web_js_node_deinit(self);
    lh_runtime_allocator_free(self);
}

lh_void
lh_web_js_node_clear(lh_web_js_node_t *self)
{
    lh_list_t *children = lh_web_js_node_get_children_mut(self);

    for (lh_list_node_t *link = lh_list_pop_front(children); lh_null_ne(link);
         link = lh_list_pop_front(children))
    {
        lh_web_js_node_destroy(lh_web_js_node_get_by_link_mut(link));
    }
    lh_str_clear(lh_web_js_node_get_name_mut(self));
    lh_str_clear(lh_web_js_node_get_text_mut(self));
    lh_web_js_node_set_flag(self, lh_bool_false);
    lh_web_js_node_set_op(self, lh_web_js_op_none);
    lh_web_js_node_set_kind(self, lh_web_js_kind_none);
}

lh_bool_t
lh_web_js_node_is_function(const lh_web_js_node_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_web_js_node_has_kind(self, lh_web_js_kind_function_expression) ||
                              lh_web_js_node_has_kind(self, lh_web_js_kind_function_declaration));
}

lh_web_js_node_t *
lh_web_js_node_create(lh_void)
{
    lh_web_js_node_t *self =
        lh_ptr_cast(lh_web_js_node_t, lh_runtime_allocator_alloc(sizeof(lh_web_js_node_t)));

    lh_web_js_node_init(self);
    return self;
}

/* ── shapes ──────────────────────────────────────────────────────────────── */

lh_void
lh_web_js_node_reset(lh_web_js_node_t *self, lh_web_js_kind_t kind)
{
    lh_web_js_node_clear(self);
    lh_web_js_node_set_kind(self, kind);
}

lh_web_js_node_t *
lh_web_js_node_push_statement(lh_web_js_node_t *self, lh_web_js_node_t *statement)
{
    lh_web_js_node_push_back(
        lh_web_js_node_is_function(self) ? lh_web_js_node_get_last_mut(self) : self, statement);
    return self;
}

lh_void
lh_web_js_node_reset_named(lh_web_js_node_t *self, lh_web_js_kind_t kind, lh_str_view_t name)
{
    lh_web_js_node_reset(self, kind);
    lh_str_assign_view(lh_web_js_node_get_name_mut(self), name);
}

lh_void
lh_web_js_node_reset_operator(lh_web_js_node_t *self, lh_web_js_kind_t kind, lh_web_js_op_t op)
{
    lh_web_js_node_reset(self, kind);
    lh_web_js_node_set_op(self, op);
}

lh_void
lh_web_js_node_set_number(lh_web_js_node_t *self, lh_ullong_t value)
{
    lh_web_js_node_reset(self, lh_web_js_kind_number);
    lh_char_t digits[LH_WEB_JS_NODE_INTEGER_DIGITS_MAX];
    lh_str_append(lh_web_js_node_get_text_mut(self), digits,
                  lh_str_ptr_format_ullong(value, digits, LH_WEB_JS_NODE_INTEGER_DIGITS_MAX));
}

lh_void
lh_web_js_node_set_string(lh_web_js_node_t *self, lh_str_view_t value)
{
    lh_web_js_node_reset(self, lh_web_js_kind_string);
    lh_str_assign_view(lh_web_js_node_get_text_mut(self), value);
}

lh_void
lh_web_js_node_set_bool(lh_web_js_node_t *self, lh_bool_t value)
{
    lh_web_js_node_reset(self, lh_web_js_kind_bool);
    lh_web_js_node_set_flag(self, value);
}

/* ── composites ──────────────────────────────────────────────────────────── */

lh_web_js_node_t *
lh_web_js_node_create_with_kind(lh_web_js_kind_t kind)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_reset(self, kind);
    return self;
}

lh_void
lh_web_js_node_set_ident(lh_web_js_node_t *self, lh_str_view_t name)
{
    lh_web_js_node_reset_named(self, lh_web_js_kind_ident, name);
}

lh_void
lh_web_js_node_set_regex(lh_web_js_node_t *self, lh_str_view_t pattern, lh_str_view_t flags)
{
    lh_web_js_node_reset_named(self, lh_web_js_kind_regex, flags);
    lh_str_assign_view(lh_web_js_node_get_text_mut(self), pattern);
}

lh_web_js_node_t *
lh_web_js_node_create_named(lh_web_js_kind_t kind, lh_str_view_t name)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_reset_named(self, kind, name);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_with_operator(lh_web_js_kind_t kind, lh_web_js_op_t op)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_reset_operator(self, kind, op);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_number(lh_ullong_t value)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_set_number(self, value);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_string(lh_str_view_t value)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_set_string(self, value);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_bool(lh_bool_t value)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_set_bool(self, value);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_function_expression(lh_void)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_function_expression);

    lh_web_js_node_push_back(self, lh_web_js_node_create_with_kind(lh_web_js_kind_block));
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_index(lh_web_js_node_t *object, lh_web_js_node_t *index)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_index);

    lh_web_js_node_push_back(self, object);
    lh_web_js_node_push_back(self, index);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_call(lh_web_js_node_t *callee)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_call);

    lh_web_js_node_push_back(self, callee);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_new(lh_web_js_node_t *callee)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_new);

    lh_web_js_node_push_back(self, callee);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_conditional(lh_web_js_node_t *test, lh_web_js_node_t *consequent,
                                  lh_web_js_node_t *alternate)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_conditional);

    lh_web_js_node_push_back(self, test);
    lh_web_js_node_push_back(self, consequent);
    lh_web_js_node_push_back(self, alternate);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_expression(lh_web_js_node_t *expression)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_expression);

    lh_web_js_node_push_back(self, expression);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_if(lh_web_js_node_t *test, lh_web_js_node_t *consequent,
                         lh_web_js_node_t *alternate)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_if);

    lh_web_js_node_push_back(self, test);
    lh_web_js_node_push_back(self, consequent);
    if (lh_null_ne(alternate))
    {
        lh_web_js_node_push_back(self, alternate);
    }
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_for(lh_web_js_node_t *init, lh_web_js_node_t *test, lh_web_js_node_t *update,
                          lh_web_js_node_t *body)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_for);

    lh_web_js_node_push_back(
        self, lh_null_ne(init) ? init : lh_web_js_node_create_with_kind(lh_web_js_kind_empty));
    lh_web_js_node_push_back(
        self, lh_null_ne(test) ? test : lh_web_js_node_create_with_kind(lh_web_js_kind_empty));
    lh_web_js_node_push_back(
        self, lh_null_ne(update) ? update : lh_web_js_node_create_with_kind(lh_web_js_kind_empty));
    lh_web_js_node_push_back(self, body);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_return(lh_web_js_node_t *argument)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_return);

    if (lh_null_ne(argument))
    {
        lh_web_js_node_push_back(self, argument);
    }
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_switch(lh_web_js_node_t *discriminant)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_switch);

    lh_web_js_node_push_back(self, discriminant);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_case(lh_web_js_node_t *test)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_case);

    lh_web_js_node_push_back(self, test);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_ident(lh_str_view_t name)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_set_ident(self, name);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_regex(lh_str_view_t pattern, lh_str_view_t flags)
{
    lh_web_js_node_t *self = lh_web_js_node_create();

    lh_web_js_node_set_regex(self, pattern, flags);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_property(lh_str_view_t key, lh_web_js_node_t *value)
{
    lh_web_js_node_t *self = lh_web_js_node_create_named(lh_web_js_kind_property, key);

    lh_web_js_node_push_back(self, value);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_function_declaration(lh_str_view_t name)
{
    lh_web_js_node_t *self = lh_web_js_node_create_named(lh_web_js_kind_function_declaration, name);

    lh_web_js_node_push_back(self, lh_web_js_node_create_with_kind(lh_web_js_kind_block));
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_member(lh_web_js_node_t *object, lh_str_view_t name)
{
    lh_web_js_node_t *self = lh_web_js_node_create_named(lh_web_js_kind_member, name);

    lh_web_js_node_push_back(self, object);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_unary(lh_web_js_op_t op, lh_web_js_node_t *operand)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_operator(lh_web_js_kind_unary, op);

    lh_web_js_node_push_back(self, operand);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_prefix(lh_web_js_op_t op, lh_web_js_node_t *operand)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_operator(lh_web_js_kind_update, op);

    lh_web_js_node_set_flag(self, lh_bool_true);
    lh_web_js_node_push_back(self, operand);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_postfix(lh_web_js_op_t op, lh_web_js_node_t *operand)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_operator(lh_web_js_kind_update, op);

    lh_web_js_node_push_back(self, operand);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_binary(lh_web_js_op_t op, lh_web_js_node_t *left, lh_web_js_node_t *right)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_operator(lh_web_js_kind_binary, op);

    lh_web_js_node_push_back(self, left);
    lh_web_js_node_push_back(self, right);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_assign(lh_web_js_op_t op, lh_web_js_node_t *target, lh_web_js_node_t *value)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_operator(lh_web_js_kind_assign, op);

    lh_web_js_node_push_back(self, target);
    lh_web_js_node_push_back(self, value);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_declarator(lh_str_view_t name, lh_web_js_node_t *init)
{
    lh_web_js_node_t *self = lh_web_js_node_create_named(lh_web_js_kind_declarator, name);

    if (lh_null_ne(init))
    {
        lh_web_js_node_push_back(self, init);
    }
    return self;
}

lh_web_js_node_t *
lh_web_js_node_create_try(lh_web_js_node_t *block, lh_str_view_t name, lh_web_js_node_t *handler)
{
    lh_web_js_node_t *self = lh_web_js_node_create_with_kind(lh_web_js_kind_try);
    lh_web_js_node_t *clause = lh_web_js_node_create_named(lh_web_js_kind_catch, name);

    lh_web_js_node_push_back(clause, handler);
    lh_web_js_node_push_back(self, block);
    lh_web_js_node_push_back(self, clause);
    return self;
}

lh_web_js_node_t *
lh_web_js_node_push_param(lh_web_js_node_t *self, lh_str_view_t name)
{
    lh_web_js_node_t *param = lh_web_js_node_create_ident(name);
    lh_web_js_node_t *body = lh_web_js_node_get_last_mut(self);

    if (lh_null_eq(body))
    {
        return lh_web_js_node_push_back(self, param);
    }
    lh_list_node_insert_before(lh_web_js_node_get_link_mut(param),
                               lh_web_js_node_get_link_mut(body));
    return self;
}

lh_web_js_node_t *
lh_web_js_node_add_declarator(lh_web_js_node_t *self, lh_str_view_t name, lh_web_js_node_t *init)
{
    return lh_web_js_node_push_back(self, lh_web_js_node_create_declarator(name, init));
}

lh_web_js_node_t *
lh_web_js_node_create_var(lh_str_view_t name, lh_web_js_node_t *init)
{
    return lh_web_js_node_add_declarator(lh_web_js_node_create_with_kind(lh_web_js_kind_var), name,
                                         init);
}
