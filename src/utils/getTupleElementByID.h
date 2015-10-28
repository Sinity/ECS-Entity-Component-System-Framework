// Based on http://stackoverflow.com/questions/16707645/how-to-get-element-from-stdtuple-by-type
#pragma once

#include <tuple>
#include <functional>

namespace detail {
    template<class T, std::size_t N, class... Args>
    struct get_number_of_element_from_tuple_by_type_impl {
        static constexpr auto value = N;
    };

    template<class T, std::size_t N, class... Args>
    struct get_number_of_element_from_tuple_by_type_impl<T, N, T, Args...> {
        static constexpr auto value = N;
    };

    template<class T, std::size_t N, class U, class... Args>
    struct get_number_of_element_from_tuple_by_type_impl<T, N, U, Args...> {
        static constexpr auto value = get_number_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
    };
}

template<class T, class... TupleElems>
T& getByType(std::tuple<TupleElems...>& t) {
    return std::get<detail::get_number_of_element_from_tuple_by_type_impl<T, 0, TupleElems...>::value>(t);
}

