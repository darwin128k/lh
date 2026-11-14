/**
 * @file layer.h
 * @brief An entity that holds other entities: ::lh_gl_layer_t.
 *
 * A layer is an ::lh_gl_entity_t (its first field, so a layer can be put into a
 * layer) plus a list of the entities it owns. It is the only kind of owner an
 * entity can have: ::lh_gl_layer_try_add makes the layer the `parent` of the
 * entity, ::lh_gl_layer_try_remove clears it again.
 *
 * **The layer owns the links, not the children.** The children are an
 * ::lh_list_t of intrusive nodes that live inside the entities themselves
 * (::lh_gl_entity_get_node). Linking and unlinking allocate nothing, so the
 * layer runs on a target without a heap or an operating system. The children
 * stay owned by the caller and must outlive their membership.
 * ::lh_gl_layer_deinit unlinks every remaining child and clears its parent, so
 * no dangling parents remain; it does not free the children.
 *
 * **List order is z-order.** The first entity is the back one, the last one is
 * the front. ::lh_gl_layer_try_add appends, so a new child is drawn on top.
 *
 * A child's `rect` is relative to the origin of the layer's own rect. A layer
 * never holds the same entity twice and never holds itself or one of its
 * ancestors, so the tree has no cycles. Whether and how a layer draws its
 * children, and what happens to the screen when a child changes, is not part of
 * this brick yet.
 *
 * Fields are private. Nobody names `entity` or `items` outside the
 * level-1 accessors; everything else, here and in callers, goes through them.
 */

#ifndef LH_GL_LAYER_H
#define LH_GL_LAYER_H

#include <lh/core/list.h>
#include <lh/gl/entity.h>
#include <lh/gl/layer/fields.h>

/**
 * @struct lh_gl_layer
 * @brief An entity plus the list of entities it owns. Fields via ::lh_gl_layer_fields.
 */
struct lh_gl_layer
{
    /** Members: ::lh_gl_layer_fields. */
    lh_gl_layer_fields(lh_gl_entity_t, lh_list_t, lh_gl_rect_t, lh_usize_t);
};

/* -- Accessors ------------------------------------------------------------- */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Mutable pointer to @p self seen as an entity.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
lh_gl_entity_t *
lh_gl_layer_get_entity_mut(lh_gl_layer_t *self);

/**
 * @brief Const pointer to @p self seen as an entity.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
const lh_gl_entity_t *
lh_gl_layer_get_entity(const lh_gl_layer_t *self);

/**
 * @brief Mutable pointer to the list of children.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
lh_list_t *
lh_gl_layer_get_items_mut(lh_gl_layer_t *self);

/**
 * @brief Const pointer to the list of children.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
const lh_list_t *
lh_gl_layer_get_items(const lh_gl_layer_t *self);

/**
 * @brief Mutable pointer to a slot in the fixed dirty rectangle storage.
 *
 * This level-1 accessor does not change the count. A slot may be prepared
 * before increasing the count with ::lh_gl_layer_set_dirty_count.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 * @param index Slot index, less than ::LH_LIBRARY_OPTION_GL_LAYER_DIRTY_MAX (asserted).
 * @return Pointer into @p self, valid as long as @p self lives. Not null.
 */
lh_gl_rect_t *
lh_gl_layer_get_dirty_mut(lh_gl_layer_t *self, lh_uindex_t index);

/** @brief Number of dirty rectangles held by this layer. */
lh_usize_t
lh_gl_layer_get_dirty_count(const lh_gl_layer_t *self);

/**
 * @brief Store the number of occupied dirty rectangle slots.
 *
 * This level-1 accessor does not initialize or clear any slots. The caller
 * must ensure the first @p count slots contain valid dirty rectangles.
 *
 * @param self Layer to change; must not be null (asserted in debug builds).
 * @param count Number of occupied slots, at most ::LH_LIBRARY_OPTION_GL_LAYER_DIRTY_MAX.
 */
lh_void
lh_gl_layer_set_dirty_count(lh_gl_layer_t *self, lh_usize_t count);

/**
 * @brief Dirty rectangle at an occupied index.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 * @param index Index below ::lh_gl_layer_get_dirty_count (asserted).
 * @return Pointer into @p self, valid until the dirty list changes. Not null.
 */
const lh_gl_rect_t *
lh_gl_layer_get_dirty(const lh_gl_layer_t *self, lh_uindex_t index);

/** @brief Clear the dirty list. */
lh_void
lh_gl_layer_clear_dirty(lh_gl_layer_t *self);

/**
 * @brief Invalidate area in this layer's coordinates.
 *
 * Clips to the layer, forwards through parents, and stores at the root only.
 * Rectangles are merged when their bounding area is no larger than their sum;
 * a full list collapses to one bounding rectangle. No memory is allocated.
 * Both arguments must be non-null. The area is copied.
 */
lh_void
lh_gl_layer_invalidate(lh_gl_layer_t *self, const lh_gl_rect_t *area);

/* -- Init and queries, by level -------------------------------------------- */

/**
 * @brief The class every layer has: kind layer, no draw call yet.
 *
 * @return A `const` table that lives for the whole program. Not null.
 */
const lh_gl_entity_class_t *
lh_gl_layer_get_class(void);

/**
 * @brief How many children @p self holds.
 *
 * O(n): the list keeps no count.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 */
lh_usize_t
lh_gl_layer_get_count(const lh_gl_layer_t *self);

/**
 * @brief Fill @p self as an empty layer.
 *
 * The entity part gets @p rect, no owner, an unlinked node and the class of a
 * layer (::lh_gl_layer_get_class). The list is empty and nothing is allocated.
 *
 * @param self Layer to fill; must not be null (asserted in debug builds).
 * @param rect Area of the layer, relative to its own parent; copied.
 */
lh_void
lh_gl_layer_init(lh_gl_layer_t *self, lh_gl_rect_t rect);

/**
 * @brief Unlink every remaining child of @p self and clear its parent.
 *
 * The children themselves are not freed. After this call the list is empty.
 * The entity part is left as it was.
 *
 * @param self Layer to deinitialize; must not be null (asserted in debug builds).
 */
lh_void
lh_gl_layer_deinit(lh_gl_layer_t *self);

/**
 * @brief True when @p self holds no children.
 *
 * @param self Layer to read; must not be null (asserted in debug builds).
 */
lh_bool_t
lh_gl_layer_is_empty(const lh_gl_layer_t *self);

/**
 * @brief True when @p index addresses a child of @p self.
 *
 * Valid indices are `0` up to, not including, ::lh_gl_layer_get_count. An index
 * outside is reported here, not raised.
 *
 * @param self  Layer to read; must not be null (asserted in debug builds).
 * @param index Index to test.
 */
lh_bool_t
lh_gl_layer_is_valid_index(const lh_gl_layer_t *self, lh_uindex_t index);

/**
 * @brief The child at @p index; list order is z-order, the last is the front.
 *
 * O(@p index): the list has no random access.
 *
 * @param self  Layer to read; must not be null (asserted in debug builds).
 * @param index Index of the child; must be valid (asserted in debug builds).
 * @return The child. Not null.
 *
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below ::lh_gl_layer_get_count.
 */
lh_gl_entity_t *
lh_gl_layer_get_item(const lh_gl_layer_t *self, lh_uindex_t index);

/**
 * @brief Look for @p entity among the children of @p self.
 *
 * Compares the nodes: only the very same entity is found. O(n).
 *
 * @param self      Layer to search; must not be null (asserted in debug builds).
 * @param entity    Entity to look for; must not be null (asserted in debug builds).
 * @param out_index Receives the index of the child when found; untouched
 *                  otherwise; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when @p entity is a child of @p self;
 *         ::lh_bool_false otherwise.
 */
lh_bool_t
lh_gl_layer_try_find(const lh_gl_layer_t *self, const lh_gl_entity_t *entity,
                     lh_uindex_t *out_index);

/**
 * @brief True when @p other is the entity of @p self or one of its ancestors.
 *
 * Walks up from @p self through the parents. Adding such an entity to @p self
 * would make the tree a cycle.
 *
 * @param self  Layer to start from; must not be null (asserted in debug builds).
 * @param other Entity to look for; must not be null (asserted in debug builds).
 */
lh_bool_t
lh_gl_layer_is_within(const lh_gl_layer_t *self, const lh_gl_entity_t *other);

/**
 * @brief Add @p entity as the front child of @p self and make @p self its parent.
 *
 * Refused, with nothing changed, when @p entity already has a parent, or when
 * @p entity is @p self's own entity or one of its ancestors
 * (::lh_gl_layer_is_within). Appends, so the new child is last in the list and
 * drawn on top.
 *
 * @param self   Layer to add to; must not be null (asserted in debug builds).
 * @param entity Entity to add, with no parent; must not be null (asserted in
 *               debug builds). It must outlive its membership.
 * @return ::lh_bool_true when added; ::lh_bool_false when refused.
 */
lh_bool_t
lh_gl_layer_try_add(lh_gl_layer_t *self, lh_gl_entity_t *entity);

/**
 * @brief Add @p entity to @p self; a refusal is a failed runtime check.
 *
 * ::lh_gl_layer_try_add followed by a check with
 * ::lh_runtime_error_code_not_accepted, for callers for whom a refusal is a bug.
 *
 * @param self   Layer to add to; must not be null (asserted in debug builds).
 * @param entity Entity to add, with no parent; must not be null (asserted in
 *               debug builds).
 */
lh_void
lh_gl_layer_add(lh_gl_layer_t *self, lh_gl_entity_t *entity);

/**
 * @brief Remove the child at @p index; the children after it keep their order.
 *
 * The removed child has no parent afterwards and its node is unlinked.
 *
 * @param self  Layer to change; must not be null (asserted in debug builds).
 * @param index Index of the child; must be valid (asserted in debug builds).
 *
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below ::lh_gl_layer_get_count.
 */
lh_void
lh_gl_layer_remove_at(lh_gl_layer_t *self, lh_uindex_t index);

/**
 * @brief Remove @p entity from the children of @p self.
 *
 * @param self   Layer to change; must not be null (asserted in debug builds).
 * @param entity Entity to remove; must not be null (asserted in debug builds).
 * @return ::lh_bool_true when it was a child and is removed (its parent is now
 *         ::lh_null); ::lh_bool_false when it is not a child of @p self.
 */
lh_bool_t
lh_gl_layer_try_remove(lh_gl_layer_t *self, lh_gl_entity_t *entity);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_LAYER_H */
