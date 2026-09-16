#include <lh/date/day.h>

lh_date_day_t
lh_date_days_in_month(lh_date_year_t year, lh_date_month_t month)
{
    static const lh_date_day_t days[LH_DATE_MONTH_INDEX_RADIX] = {31, 28, 31, 30, 31, 30,
                                                                 31, 31, 30, 31, 30, 31};
    lh_date_month_index_t index;

    if (month < LH_DATE_MONTH_MIN || month > LH_DATE_MONTH_MAX)
    {
        return 0;
    }

    index = lh_date_month_to_index(month);
    if (index == LH_DATE_MONTH_INDEX_FEBRUARY && lh_date_year_is_leap(year))
    {
        return 29;
    }
    return days[index];
}

lh_date_day_t
lh_date_days_left(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day)
{
    lh_date_day_t dim = lh_date_days_in_month(year, month);

    if (dim == 0 || day < LH_DATE_DAY_MIN || day > dim)
    {
        return 0;
    }
    return (lh_date_day_t)(dim - day);
}

lh_date_day_t
lh_date_days_left_with_today(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day)
{
    lh_date_day_t dim = lh_date_days_in_month(year, month);

    if (dim == 0 || day < LH_DATE_DAY_MIN || day > dim)
    {
        return 0;
    }
    return (lh_date_day_t)((dim - day) + 1U);
}
