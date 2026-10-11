/*
 * lv_conf.h -- LVGL for the comparison: its software renderer into a 32-bit buffer, no
 * OS, no theme (a theme would add looks lh is not asked to draw), and the features the
 * scenes use. Everything not named here is LVGL's own default.
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH 32
#define LV_USE_OS LV_OS_NONE
#define LV_MEM_SIZE (4U * 1024U * 1024U)

#define LV_USE_DRAW_SW 1
#define LV_DRAW_SW_COMPLEX 1
#define LV_USE_DRAW_SW_COMPLEX_GRADIENTS 1
#define LV_DRAW_SW_DRAW_UNIT_CNT 1
#define LV_USE_DRAW_SW_ASM LV_DRAW_SW_ASM_NONE

#define LV_USE_THEME_DEFAULT 0
#define LV_USE_THEME_SIMPLE 0
#define LV_USE_LOG 0
#define LV_USE_ASSERT_NULL 0
#define LV_USE_ASSERT_MALLOC 0
#define LV_USE_SYSMON 0

#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_DEFAULT &lv_font_montserrat_16

#define LV_USE_FLEX 1
#define LV_USE_GRID 0

#endif /* LV_CONF_H */
