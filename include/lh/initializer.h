#ifndef LH_INITIALIZER_H
#define LH_INITIALIZER_H

#define lh_initializer(...)                                                    \
    { __VA_ARGS__ }

#define lh_initializer_of(T, initializer, ...) ((T)initializer(__VA_ARGS__))
#define lh_initializer_of_type(T, ...)                                         \
    lh_initializer_of(T, lh_initializer, __VA_ARGS__)
#define lh_initializer_of_struct(T, ...)                                       \
    lh_initializer_of_type(struct T, __VA_ARGS__)

#endif // LH_INITIALIZER_H