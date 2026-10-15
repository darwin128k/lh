/**
 * @file out.h
 * @brief Test helper: call a function that writes its result through a trailing
 *        out-parameter and get that result back as a value.
 *
 * `lh_test::out(f, a, b)` calls `f(a, b, &result)` and returns `result`, so a
 * getter stays usable inside an expectation. The result type comes from the
 * last parameter of `f`.
 */

#ifndef LH_TEST_OUT_H
#define LH_TEST_OUT_H

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace lh_test
{

template <typename... P>
using out_t = typename std::remove_pointer<
    typename std::tuple_element<sizeof...(P) - 1, std::tuple<P...>>::type>::type;

template <typename... P, typename... A>
out_t<P...>
out(void (*fn)(P...), A &&...args)
{
    out_t<P...> result{};
    fn(std::forward<A>(args)..., &result);
    return result;
}

} // namespace lh_test

#endif // LH_TEST_OUT_H
