/**
 * @file key.h
 * @brief Key codes carried by ::LH_ENTITY_EVENT_KEY.
 *
 * A value below 128 that is not one of the names below is a character.
 */

#ifndef LH_ENTITY_KEY_H
#define LH_ENTITY_KEY_H

/**
 * @def LH_ENTITY_KEY_BACKSPACE
 * @brief Delete the character before the cursor.
 */
#define LH_ENTITY_KEY_BACKSPACE 8U

/**
 * @def LH_ENTITY_KEY_ENTER
 * @brief A new line in a multiline field. A single line ignores it.
 */
#define LH_ENTITY_KEY_ENTER 13U

/**
 * @def LH_ENTITY_KEY_LEFT
 * @brief Move the cursor one character toward the start.
 */
#define LH_ENTITY_KEY_LEFT 0x11U

/**
 * @def LH_ENTITY_KEY_RIGHT
 * @brief Move the cursor one character toward the end.
 */
#define LH_ENTITY_KEY_RIGHT 0x12U

/**
 * @def LH_ENTITY_KEY_DELETE
 * @brief Delete the character after the cursor.
 */
#define LH_ENTITY_KEY_DELETE 0x7FU

#endif /* LH_ENTITY_KEY_H */
