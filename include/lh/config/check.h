/**
 * @file check.h
 * @brief Compile-time validation of LH configuration values.
 *
 * Included by <lh/config.h> after its option definitions. Invalid values are
 * rejected here for both generated and manually maintained configurations.
 */

#ifndef LH_CONFIG_CHECK_H
#define LH_CONFIG_CHECK_H

#if LH_LIBRARY_OPTION_GL_LAYER_DIRTY_MAX < 1
#    error LH_LIBRARY_OPTION_GL_LAYER_DIRTY_MAX must be positive
#endif

#endif /* LH_CONFIG_CHECK_H */
