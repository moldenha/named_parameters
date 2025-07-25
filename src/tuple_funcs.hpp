#ifndef NT_NAMED_ARGUMENT_PARAMETERS_TUPLE_FUNCS_HPP__
#define NT_NAMED_ARGUMENT_PARAMETERS_TUPLE_FUNCS_HPP__

#include <tuple>
#include <utility> //integer sequence stuff

namespace nt{
namespace named_arguments{

namespace detail {
template<class T, T I, T N, T... integers>
struct make_integer_sequence_helper
{
    using type = typename make_integer_sequence_helper<T, I + 1, N, integers..., I>::type;
};
 
template<class T, T N, T... integers>
struct make_integer_sequence_helper<T, N, N, integers...>
{
    using type = std::integer_sequence<T, integers...>;
};
}

template<std::size_t N, typename Tuple, typename Indices = std::make_index_sequence<N>>
struct nt_first_n_tuple_types_;

template<std::size_t N, typename Tuple, std::size_t... I>
struct nt_first_n_tuple_types_<N, Tuple, std::index_sequence<I...>>{
    using type = std::tuple<std::tuple_element_t<I, Tuple>...>;
};

// Template for selecting the last tuple types from a given starting index
template<std::size_t Start, typename Tuple, typename Indices = detail::make_integer_sequence_helper<std::size_t, Start, std::tuple_size_v<Tuple>>>
struct nt_last_tuple_types_from_start_;

// Specialization of the template for when an index sequence is available
template<std::size_t Start, typename Tuple, std::size_t... I>
struct nt_last_tuple_types_from_start_<Start, Tuple, std::integer_sequence<std::size_t, I...>> {
    using type = std::tuple<std::tuple_element_t<I, Tuple>...>;
};

// Recursive template that removes the first element of a tuple
template <typename Tuple>
struct nt_remove_first_tuple_type_impl;

// Specialization for the case where the tuple has at least one element
template <typename Head, typename... Tail>
struct nt_remove_first_tuple_type_impl<std::tuple<Head, Tail...>> {
    using type = std::tuple<Tail...>; // Remove the first element and recursively build the new tuple
};

// Specialization for when the tuple is empty
template <>
struct nt_remove_first_tuple_type_impl<std::tuple<>> {
    using type = std::tuple<>;  // An empty tuple if there are no elements left
};


// Main struct to remove the first element of a tuple
template <typename Tuple>
using nt_remove_first_tuple_type_t = typename nt_remove_first_tuple_type_impl<Tuple>::type;


namespace details{
//std::__tuple_cat_return is only compatible for gcc
//so instead I am using this:
template<typename ... tuples_t>
using nt__tuple_cat_return_t =
decltype(std::tuple_cat(
    std::declval<tuples_t>()...
));


}

template<std::size_t N, typename n_type, typename Tuple>
using nt_replace_tuple_type_t = typename details::nt__tuple_cat_return_t<
                                        typename nt_first_n_tuple_types_<N-1, Tuple>::type,
                                        std::tuple<n_type>,
                                        typename nt_last_tuple_types_from_start_<N+1, Tuple>::type>;



}}
//::nt::named_arguments


#endif
