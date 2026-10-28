/**
 * @file kind.h
 * @brief ::lh_ui_web_x_kind_t — what an ::lh_ui_web_x_t node is, and so which member of its union is set.
 */

#ifndef LH_UI_WEB_X_KIND_H
#define LH_UI_WEB_X_KIND_H

/**
 * @enum lh_ui_web_x_kind
 * @brief What an ::lh_ui_web_x_t node is, and so which member of its union is set.
 */
typedef enum lh_ui_web_x_kind
{
    lh_ui_web_x_kind_id = 0,  /**< A bare identifier, written as is. */
    lh_ui_web_x_kind_num,     /**< A whole number. */
    lh_ui_web_x_kind_str,     /**< A string literal, quoted and escaped. */
    lh_ui_web_x_kind_null,    /**< `null`. */
    lh_ui_web_x_kind_true,    /**< `true`. */
    lh_ui_web_x_kind_false,   /**< `false`. */
    lh_ui_web_x_kind_dot,     /**< `(obj).name`. */
    lh_ui_web_x_kind_idx,     /**< `(obj)[ix]`. */
    lh_ui_web_x_kind_call,    /**< `(fn)(args...)`. */
    lh_ui_web_x_kind_call_id, /**< `name(args...)`. */
    lh_ui_web_x_kind_bin,     /**< `(a)op(b)`. */
    lh_ui_web_x_kind_un,      /**< `op(a)`. */
    lh_ui_web_x_kind_tern,    /**< `(c)?(t):(f)`. */
    lh_ui_web_x_kind_typeof,  /**< `typeof(a)`. */
    lh_ui_web_x_kind_obj      /**< `{key:value,...}`. */
} lh_ui_web_x_kind_t;         /**< What an ::lh_ui_web_x_t node is. */

#endif /* LH_UI_WEB_X_KIND_H */
