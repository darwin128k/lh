#include <lh/date/day.h>
#include <lh/cast/static.h>

static lh_date_day_t
lh_date_day_dim_if_valid(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day)
{
    lh_date_day_t dim = lh_date_days_in_month(year, month);

    if (dim == 0 || day < LH_DATE_DAY_MIN || day > dim)
    {
        return 0;
    }

    return dim;
}

lh_date_day_t
lh_date_days_in_month(lh_date_year_t year, lh_date_month_t month)
{
    static const lh_date_day_t days[LH_DATE_MONTH_INDEX_RADIX] = {
        LH_DATE_DAY_MAX,       LH_DATE_DAY_FEBRUARY, LH_DATE_DAY_MAX,   LH_DATE_DAY_SHORT,
        LH_DATE_DAY_MAX,       LH_DATE_DAY_SHORT,    LH_DATE_DAY_MAX,   LH_DATE_DAY_MAX,
        LH_DATE_DAY_SHORT,     LH_DATE_DAY_MAX,      LH_DATE_DAY_SHORT, LH_DATE_DAY_MAX};
    lh_date_month_index_t index;

    if (month < LH_DATE_MONTH_MIN || month > LH_DATE_MONTH_MAX)
    {
        return 0;
    }

    index = lh_date_month_to_index(month);
    if (index == LH_DATE_MONTH_INDEX_FEBRUARY && lh_date_year_is_leap(year))
    {
        return LH_DATE_DAY_FEBRUARY_LEAP;
    }
    return days[index];
}

lh_date_day_t
lh_date_days_left(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day)
{
    lh_date_day_t dim = lh_date_day_dim_if_valid(year, month, day);

    if (dim == 0)
    {
        return 0;
    }

    return lh_cast_static(lh_date_day_t, (dim - day));
}

lh_date_day_t
lh_date_days_left_with_today(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day)
{
    lh_date_day_t dim = lh_date_day_dim_if_valid(year, month, day);

    if (dim == 0)
    {
        return 0;
    }

    return lh_cast_static(lh_date_day_t, ((dim - day) + 1U));
}
