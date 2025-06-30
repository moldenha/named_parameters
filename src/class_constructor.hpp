#ifndef NT_NAMED_ARGUMENT_PARAMETERS_CLASS_CONSTRUCTOR_HPP__
#define NT_NAMED_ARGUMENT_PARAMETERS_CLASS_CONSTRUCTOR_HPP__

#include "function_wrap.hpp"
#include "overload_op.hpp"
#include "tuple_funcs.hpp"
#include "underscore_op.hpp"
#include "macros.h"
#include <iostream>
#include <tuple>

namespace nt{
namespace named_arguments{

template<typename overload_type, typename ArgHolder, std::size_t N, typename T, T... values, typename... Args>
inline constexpr decltype(auto) nt_get_proper_classnth_element_(std::integer_sequence<T, values...> seq, ArgHolder& holder, Args&&... args){
    using out_type = typename std::tuple_element_t<N, overload_type>;
    if constexpr (!is_in(N, seq)){
        static_assert(ArgHolder::template has_default<N>(), "Missing required positional argument");
        // return nt_prepare_tuple_element(std::forward<decltype(holder.template get<N>())>(holder.template get<N>()));

        auto equal_op = holder.template get<N>();
        using type = typename decltype(equal_op)::Type;
        if constexpr (std::is_same_v<type_details::remove_cvref_t<out_type>, nt_auto_>){
            return std::forward<type>(equal_op.val);
        }else{
            return static_cast<out_type>(std::forward<type>(equal_op.val.get()));
        }
    }else{
        constexpr std::size_t new_N = nt_get_propernth_element_index_<N, 0, T, values...>(seq);
        if constexpr (std::is_same_v<type_details::remove_cvref_t<out_type>, nt_auto_>){
            return nt_prepare_tuple_element_(std::get<new_N>(std::forward_as_tuple(std::forward<Args>(args)...)));
        }else{
            return static_cast<out_type>(nt_prepare_tuple_element_(std::get<new_N>(std::forward_as_tuple(std::forward<Args>(args)...))));
        }
        // return static_cast<OutType>(nt_getnth_element_<new_N>(std::forward<Args>(args)...));
    }
}


template<typename overload_type, typename ArgHolder, typename T, T... values, T... values2, typename... Args>
inline constexpr typename overload_type::param_tuple_type nt_get_class_arg_tuple_(std::integer_sequence<T, values2...>, std::integer_sequence<T, values...> seq, ArgHolder& holder, Args&&... args) {
    using out_type = typename overload_type::param_tuple_type;
    return out_type{nt_get_proper_classnth_element_<out_type, ArgHolder, values2>(seq, holder, std::forward<Args>(args)...)...};
}
template <typename ArgHolder, typename overload_type, typename... Args>
inline constexpr decltype(auto) make_constructable_tuple(ArgHolder &&arg_holder,
                                               overload_type &&overload_holder,
                                               Args &&...args) {
    // if constexpr (sizeof...(Args) == 
    constexpr auto indices = std::index_sequence_for<Args...>{};
    constexpr auto sequence =
        create_index_sequence_helper<ArgHolder, Args...>(indices);
    static_assert(has_duplicates(sequence) == false,
                  "Error: mismatch of arguments with positioned and "
                  "non-positioned arguments");
    constexpr auto arg_sequence = ArgHolder::make_index_sequence();
    static_assert(
        holds_all_required<ArgHolder>(sequence, arg_sequence),
        "Positional arguments without default value have been not filled in");
    return nt_get_class_arg_tuple_<overload_type>(arg_sequence, sequence, arg_holder,
                                       std::forward<Args>(args)...);
}


}} // ::nt::named_arguments





// class MyClass {
//     int a, b, c;
//     double d;

//   public:
//         NT_MAKE_NAMED_CLASS_CONSTRUCTOR(MyClass, 
//                         NT_NAMED_CLASS_CONSTCUTOR_CLASS_ARG_NAMES_(a, b, c, d),
//                         NT_NAMED_CLASS_CONSTCUTOR_CLASS_DEFAULT_VALS_(20, 20.32),
//                         NT_NAMED_CLASS_CONSTCUTOR_CLASS_ARG_TYPES_(int, int, int, double))
//         :a(a), b(b), c(c), d(d) {}
//         // template<typename... Args>
//         // MyClass(Args&&... args)
//         // :MyClass(std::make_from_tuple<MyClass>(make_constructable_tuple(
//         //     make_arg_holder(
//         //         ntarg_(a),
//         //         ntarg_(b),
//         //         ntarg_(c) = 20,
//         //         ntarg_(d) = 20.32),
//         //         nt_overload_operator_<void, int, int, int, double>{},
//         //     std::forward<Args>(args)...))) {;}

//         // MyClass(int a, int b, int c, double d)
//         // :a(a), b(b), c(c), d(d) {}
//     void print(){
//         std::cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d
//                   << std::endl;}
// };

#endif
