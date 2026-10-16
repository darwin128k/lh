/* lh/ui.h compiled as C11: the library is C, so its umbrella must be too. */
#include <lh/ui.h>

int
lh_test_ui_umbrella_c(void);

int
lh_test_ui_umbrella_c(void)
{
    lh_byte_t mem[8];
    lh_ui_web_buf_t buf;
    lh_ui_web_buf_init(&buf, mem, sizeof(mem));
    return lh_ui_web_buf_ok(&buf) && lh_ui_gradient_sine(0) == 0;
}
