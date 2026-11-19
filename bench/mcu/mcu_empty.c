/* bench/mcu/mcu_empty.c -- the baseline: a program that does nothing, linked
   with the same C library as mcu_gx.c. What it takes is not gx's. */

int
main(void)
{
    for (;;)
    {
    }
}
