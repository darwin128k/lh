/**
 * @file error.h
 * @brief ::lh_ui_decl_error_t — what a build refused, and the one question asked of every row.
 */

#ifndef LH_UI_DECL_ERROR_H
#define LH_UI_DECL_ERROR_H

/**
 * @enum lh_ui_decl_error
 * @brief What a build refused, and the one question asked of every row.
 */
typedef enum lh_ui_decl_error
{
    lh_ui_decl_error_none = 0, /**< It built. */
    lh_ui_decl_error_kind,     /**< @c kind is not a widget this build knows. */
    lh_ui_decl_error_parent,   /**< @c parent is out of range, or not lower than this row. */
    lh_ui_decl_error_roots,    /**< Not exactly one row is a root. */
    lh_ui_decl_error_box,    /**< A scrollbar names a row that is not a container, or is higher. */
    lh_ui_decl_error_storage /**< The pool is too small for these rows. */
} lh_ui_decl_error_t;        /**< What a build refused. */

#endif /* LH_UI_DECL_ERROR_H */
