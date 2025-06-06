#ifndef __NT_NAMED_ARGUMENT_PARAMETERS_FUNCTION_WRAP_HPP__
#define __NT_NAMED_ARGUMENT_PARAMETERS_FUNCTION_WRAP_HPP__

#include <tuple>
#include "underscore_op.hpp"
#include <functional>
#include "tuple_funcs.hpp"

namespace nt{
namespace named_arguments{

template<typename Arg1, typename Arg2, typename... Args>
struct _nt_args_are_well_formed_{
    static constexpr bool value = !(__NT_IsEqualReflectOp__V<Arg1> && is_nt_string_literal_v<Arg2>) && _nt_args_are_well_formed_<Arg2, Args...>::value;
};

template<typename Arg1, typename Arg2>
struct _nt_args_are_well_formed_<Arg1, Arg2>{
    static constexpr bool value = !(__NT_IsEqualReflectOp__V<Arg1> && is_nt_string_literal_v<Arg2>);
};

// Base case specialization when there's only one type.
template<typename Arg1>
struct _nt_args_are_well_formed_<_NT_StringLiteral_<'h'>, Arg1>{
    static constexpr bool value = true;
};

// Special case for the `_NT_StringLiteral_` template.
template<>
struct _nt_args_are_well_formed_<_NT_StringLiteral_<'h'>, _NT_StringLiteral_<'h'>>{
    static constexpr bool value = true;
};

template<typename T, std::size_t cur, char... Chars>
inline constexpr std::size_t _nt_arg_holder_get_index_(const _NT_StringLiteral_<Chars...>& lit_str){
    if constexpr (cur >= T::size){return T::size;}
    else{
        constexpr auto literal = T::template get_literal<cur>();
        if constexpr (decltype(literal)::isEqual(_NT_StringLiteral_<Chars...>{})){return cur;}
        return _nt_arg_holder_get_index_<T, cur+1, Chars...>(lit_str);
    }
}

template<typename Tuple, typename... DefaultArgs, std::size_t... Is>
inline constexpr decltype(auto) _nt_set_tuple_args_(Tuple& t, std::index_sequence<Is...>, DefaultArgs&&... args){
    return std::make_tuple((std::get<Is>(t) = std::forward<DefaultArgs>(args))...);
}

template<typename... Args>
struct __NT_arg_holder__{
    static_assert(_nt_all_string_literals_<Args...>::value, "Expected all string literal types for args holder");
    static_assert(sizeof...(Args) > 0, "Expected to have multiple arguments for argument holder");
    static_assert(sizeof...(Args) < 2 || _nt_args_are_well_formed_<_NT_StringLiteral_<'h'>, Args...>::value, "Expected arguments to follow from non-default to default");
    std::tuple<Args...> args;
    
    constexpr __NT_arg_holder__(Args&&... tup) : args(std::make_tuple(std::forward<Args>(tup)...)) {}
    constexpr __NT_arg_holder__(std::tuple<Args...> tup) : args(tup) {}
    
    static constexpr std::size_t size = sizeof...(Args);
    
    template<typename... NArgs>
    inline static constexpr __NT_arg_holder__<NArgs...> from_tuple(std::tuple<NArgs...> tup){return __NT_arg_holder__<NArgs...>(std::move(tup));}
    template<std::size_t i>
    inline constexpr auto get(){return std::get<i>(args);}

    template<std::size_t i>
    inline static constexpr auto get_literal(){
        if constexpr (__NT_IsEqualReflectOp__V<typename std::tuple_element<i, std::tuple<Args...>>::type>){
            return std::tuple_element<i, std::tuple<Args...>>::type::literal();
        }else{
            return typename std::tuple_element<i, std::tuple<Args...>>::type{};
        }
    }

    template<std::size_t i>
    inline static constexpr bool has_default(){
        return __NT_IsEqualReflectOp__V<typename std::tuple_element<i, std::tuple<Args...>>::type>;
    }

    template<std::size_t i>
    inline static constexpr bool is_required(){return !has_default<i>();}

    template<char... Chars>
    inline static constexpr std::size_t get_index(_NT_StringLiteral_<Chars...> str){
        constexpr std::size_t cur = _nt_arg_holder_get_index_<__NT_arg_holder__<Args...>, 0, Chars...>(str);
        static_assert(cur < sizeof...(Args), "Error, literal string provided for argument does not exist, invalid argument name");
        return cur;
    }

    template<typename T, char... Chars>
    inline static constexpr std::size_t get_index(__NT_EqualReflectOp__<T, Chars...> str){return get_index(decltype(str)::literal());}
    
    inline static constexpr auto make_index_sequence() {return std::index_sequence_for<Args...>{};}

    template<typename... DefaultArgs>
    inline constexpr auto define_last_defaults(DefaultArgs&&... d_args){
        static_assert(sizeof...(DefaultArgs) <= sizeof...(Args),
                      "Cannot have more default arguments than available arguments");
        constexpr std::size_t num_args = sizeof...(Args);
        using sequence_type = typename detail::make_integer_sequence_helper<std::size_t, num_args-sizeof...(DefaultArgs), num_args>::type;
        auto ending_tuple = _nt_set_tuple_args_(args, sequence_type{}, std::forward<DefaultArgs>(d_args)...);
        typename _nt_first_n_tuple_types_<num_args-sizeof...(DefaultArgs), std::tuple<Args...>>::type beggining;
        auto out_tuple = std::tuple_cat(beggining, ending_tuple);
        return from_tuple(std::move(out_tuple));
    }
};




template<typename... Args>
inline constexpr __NT_arg_holder__<Args...> make_arg_holder(Args&&... args){
    return __NT_arg_holder__<Args...>(std::forward<Args>(args)...);
}

//make sure a positional argument does not come after a named one
template<typename Arg1, typename Arg2, typename... Args>
struct _nt_incoming_func_args_are_well_formed_{
    static constexpr bool value = !is_nt_string_literal_v<Arg1> 
                && !(__NT_IsEqualReflectOp__V<Arg1> && !__NT_IsEqualReflectOp__V<Arg2>) 
                && _nt_incoming_func_args_are_well_formed_<Arg2, Args...>::value;
};

template<typename Arg1, typename Arg2>
struct _nt_incoming_func_args_are_well_formed_<Arg1, Arg2>{
    static constexpr bool value = !is_nt_string_literal_v<Arg1> 
                && !(__NT_IsEqualReflectOp__V<Arg1> && !__NT_IsEqualReflectOp__V<Arg2>);
};

// Base case specialization when there's only one type.
template<typename Arg1>
struct _nt_incoming_func_args_are_well_formed_<Arg1, Arg1>{
    static constexpr bool value = !is_nt_string_literal_v<Arg1>;
};

// Special case for the `_NT_StringLiteral_` template.
template<>
struct _nt_incoming_func_args_are_well_formed_<int, int>{
    static constexpr bool value = true;
};

/*
example:
template<typename... Args>
void foo(Args... args){
    static_assert(_nt_incoming_func_args_are_well_formed_<Args...>::value, "Positional arguments must all come before named ones, and all arguments must be defined");

}
*/



template<std::size_t cur, typename T, typename Arg>
inline constexpr std::size_t get_arg_index(){
    if constexpr (!__NT_IsEqualReflectOp__V<Arg>){
        return cur;
    }else{
        constexpr std::size_t index = std::remove_reference_t<std::remove_cv_t<T>>::get_index(std::remove_reference_t<std::remove_cv_t<Arg>>::literal());
        //static_assert(index >= cur, "Cannot have a named argument that comes before a positional one");
        return index;
    }
}

template<std::size_t cur, typename T, typename Arg>
struct _nt_get_arg_index_{
    
    static constexpr std::size_t value = (__NT_IsEqualReflectOp__V<Arg>) 
                    ? std::remove_reference_t<std::remove_cv_t<T>>::get_index(std::remove_reference_t<std::remove_cv_t<Arg>>::literal())
                    : cur + 1;
};

// Helper to expand arguments and create an integer sequence
template <typename ArgHolder, typename... Args, std::size_t... I>
inline constexpr auto create_index_sequence_helper(std::index_sequence<I...>) {
    // Expanding each argument and generating the corresponding index
    return std::integer_sequence<std::size_t, (get_arg_index<I, ArgHolder, typename std::tuple_element<I, std::tuple<Args...>>::type>())...>{};
}


template <typename T, T ... Is>
inline constexpr bool is_in(T i, std::integer_sequence<T, Is...>)
{
    return ((i == Is) || ...);
}

template<typename T, T val, T... values>
inline constexpr bool has_duplicates(std::integer_sequence<T, val, values...>){
    if constexpr(sizeof...(values) == 0){return false;}
    else if constexpr (is_in(val, std::integer_sequence<T, values...>{})){return true;}
    else{return has_duplicates(std::integer_sequence<T, values...>{});}
}

template<typename T>
inline constexpr bool has_duplicates(std::integer_sequence<T>){return false;}


template<typename ArgHolder, typename T, T... values, T val2, T... values2>
inline constexpr bool holds_all_required(std::integer_sequence<T, values...> seq, std::integer_sequence<T, val2, values2...>){
    if constexpr (ArgHolder::template has_default<val2>()){return true;}
    else if constexpr(ArgHolder::template is_required<val2>() && !is_in(val2, seq)){return false;}
    else if constexpr(sizeof...(values2) == 0){return true;}
    else{return holds_all_required<ArgHolder>(seq, std::integer_sequence<T, values2...>{});} 
}

template<typename T, typename Arg>
inline constexpr decltype(auto) add_cv_like(Arg& arg) noexcept {
    if constexpr (std::is_const<T>::value && std::is_volatile<T>::value) {
        return const_cast<const volatile Arg&>(arg);
    }
    else if constexpr (std::is_const<T>::value) {
        return const_cast<const Arg&>(arg);
    }
    else if constexpr (std::is_volatile<T>::value) {
        return const_cast<volatile Arg&>(arg);
    }
    else {
        return const_cast<Arg&>(arg);
    }
}

template<typename T>
struct _nt_define_reflect_type_{
    using static_type = typename std::remove_reference_t<std::remove_cv_t<T>>;
    using type = typename std::conditional_t<
                    std::is_reference_v<T>, 
                        std::conditional_t<std::is_const_v<T>,
                            std::conditional_t<std::is_volatile_v<T>, const volatile static_type&, const static_type&>, //const volatile
                        std::conditional_t<std::is_volatile_v<T>,  volatile static_type&, static_type&>>,            //is_const_v
                    static_type>;
};

template<typename T, char... Chars>
struct _nt_define_reflect_type_<__NT_EqualReflectOp__<T, Chars...>>{
    using static_type = typename _nt_define_reflect_type_<T>::static_type; 
    using type = typename _nt_define_reflect_type_<T>::type; 
};


template<typename T, char... Chars>
inline constexpr typename _nt_define_reflect_type_<T>::type _nt_prepare_tuple_element_(__NT_EqualReflectOp__<T, Chars...>&& element){
    if constexpr(std::is_reference<T>::value){
        return add_cv_like<typename _nt_define_reflect_type_<T>::type>(std::forward<typename _nt_define_reflect_type_<T>::type>(element.val));
    }else{
        return std::forward<typename _nt_define_reflect_type_<T>::static_type>(element.val);
    }
}

template<typename T>
inline constexpr typename _nt_define_reflect_type_<T>::type _nt_prepare_tuple_element_(T&& element){
    if constexpr (std::is_reference<T>::value){
        return add_cv_like<typename _nt_define_reflect_type_<T>::type>(std::forward<typename _nt_define_reflect_type_<T>::type>(element));
    }else{
        return std::forward<typename _nt_define_reflect_type_<T>::static_type>(element);
    }
}



template <std::size_t I, std::size_t N, std::size_t... R>
struct _nt_get_nth_sequence_element_ : _nt_get_nth_sequence_element_ <I - 1, R...> { };

template <std::size_t N, std::size_t... R>
struct _nt_get_nth_sequence_element_ <0, N, R...> : std::integral_constant <std::size_t, N> { };


template<typename... Args>
struct _nt_make_exact_type_tuple_type_impl_{
    using Type = std::tuple<Args...>;
};

template<typename Arg1, typename... Args>
struct _nt_make_exact_type_tuple_type_{
    using Type = typename _nt_make_exact_type_tuple_type_impl_<typename _nt_define_reflect_type_<Arg1>::type, typename _nt_define_reflect_type_<Args>::type... >::Type;
};


template<std::size_t N, typename... Args>
inline constexpr auto _nt_get_nth_element_(Args&&... args){
    return std::get<N>((typename _nt_make_exact_type_tuple_type_<Args...>::Type){_nt_prepare_tuple_element_(std::forward<Args>(args))...});
}



template<std::size_t N, std::size_t cur, typename T, T... values>
inline constexpr std::size_t _nt_get_proper_nth_element_index_(std::integer_sequence<T, values...> seq){
    if constexpr(cur >= sizeof...(values)){return 0;}
    else if constexpr (_nt_get_nth_sequence_element_<cur, values...>::value == N){return cur;}
    else{
        return _nt_get_proper_nth_element_index_<N, cur+1>(seq);
    }
}

template<typename ArgHolder, std::size_t N, typename Seq, typename... Args>
struct _nt_define_nth_tuple_reflect_type_{
    //static constexpr std::index_sequence<std::size_t, values...> seq;
    using cur_type = 
        std::conditional_t<is_in(N, Seq{}),
            typename std::tuple_element_t<_nt_get_proper_nth_element_index_<N, 0>(Seq{}), typename _nt_make_exact_type_tuple_type_<Args...>::Type>,
            typename _nt_define_reflect_type_<std::tuple_element_t<(N >= ArgHolder::size ? 0 : N), decltype(ArgHolder::args)>>::type>;
};

template<typename ArgHolder, std::size_t N, typename T, T... values, typename... Args>
inline constexpr typename _nt_define_nth_tuple_reflect_type_<ArgHolder, N, std::index_sequence<values...>, Args...>::cur_type
    _nt_get_proper_nth_element_ (std::integer_sequence<T, values...> seq, ArgHolder& holder, Args&&... args){
    if constexpr (!is_in(N, seq)){
        static_assert(ArgHolder::template has_default<N>(), "Missing required positional argument");
        // return _nt_prepare_tuple_element(std::forward<decltype(holder.template get<N>())>(holder.template get<N>()));
        auto equal_op = holder.template get<N>();
        using type = typename decltype(equal_op)::Type;
        return std::forward<type>(equal_op.val);
    }else{
        using OutType = typename _nt_define_nth_tuple_reflect_type_<ArgHolder, N, decltype(seq), Args...>::cur_type;
        constexpr std::size_t new_N = _nt_get_proper_nth_element_index_<N, 0, T, values...>(seq);
        return static_cast<OutType>(_nt_prepare_tuple_element_(std::get<new_N>(std::forward_as_tuple(std::forward<Args>(args)...))));
        // return static_cast<OutType>(_nt_get_nth_element_<new_N>(std::forward<Args>(args)...));
    }
}




template<std::size_t... values>
struct _nt_first_index_sequence_element_impl_{
    static constexpr std::size_t value = 0;
};

template<std::size_t val, std::size_t... values>
struct _nt_first_index_sequence_element_impl_<val, values...>{
    static constexpr std::size_t value = val;
};



template<std::size_t... values>
inline static constexpr std::size_t first_index_sequence_element(std::index_sequence<values...>){return _nt_first_index_sequence_element_impl_<values...>::value;}

template<std::size_t value, std::size_t... values>
inline static constexpr std::index_sequence<values...> remove_first_element_from_sequence(std::index_sequence<value, values...>){return std::index_sequence<values...>{};}

template<typename Seq>
using remove_first_element_from_sequence_t = decltype(remove_first_element_from_sequence(Seq{})); 

// Specialization
template<typename ArgHolder, typename Seq, typename Seq2, typename... Args>
struct _nt_find_proper_function_arg_tuple_type_{
    static constexpr std::size_t Val1 = first_index_sequence_element(Seq2{}); 
    using type = typename std::conditional_t<Val1 >= ArgHolder::size,
                std::tuple<>,
                typename details::nt__tuple_cat_return_t<std::tuple<typename _nt_define_nth_tuple_reflect_type_<ArgHolder, Val1, Seq, Args...>::cur_type>,
                typename _nt_find_proper_function_arg_tuple_type_<ArgHolder, Seq, remove_first_element_from_sequence_t<Seq2>, Args...>::type> >;
    

};

template<typename ArgHolder, typename Seq, typename... Args>
struct _nt_find_proper_function_arg_tuple_type_<ArgHolder, Seq, std::index_sequence<>, Args...>{
    using type = std::tuple<>;
};


// Tuple packing and accessing values
template<typename ArgHolder, typename T, T... values, T... values2, typename... Args>
inline constexpr typename _nt_find_proper_function_arg_tuple_type_<ArgHolder, std::integer_sequence<T, values...>, std::integer_sequence<T, values2...>, Args...>::type
_nt_get_function_arg_tuple_(std::integer_sequence<T, values2...>, std::integer_sequence<T, values...> seq, ArgHolder& holder, Args&&... args) {
    using type = typename _nt_find_proper_function_arg_tuple_type_<ArgHolder, std::integer_sequence<T, values...>, std::integer_sequence<T, values2...>, Args...>::type;
    return type{_nt_get_proper_nth_element_<ArgHolder, values2>(seq, holder, std::forward<Args>(args)...)...};
    // return std::make_tuple(
    //     _nt_get_proper_nth_element_<ArgHolder, values2>(seq, holder, std::forward<Args>(args)...)...
    // );
}


}}
//::nt::named_arguments


// void foo_impl(double a, int b, int c, double d){
//     std::cout << "second implementation"<<std::endl;
// }

// void foo_impl(int a, int b, int c, double d){
//     std::cout << "first implementation"<<std::endl;
//     std::cout << "a: "<<a<<" b: "<<b<<" c: "<<c<<" d: "<<d<<std::endl;
// }

// template<typename... Args>
// void foo_func_test(Args&&... args){
//     static_assert(_nt_incoming_func_args_are_well_formed_<Args...>::value, "Positional arguments must all come before named ones, and all arguments must be defined");
//     auto arg_holder = make_arg_holder(
//         ntarg_(a),
//         ntarg_(b),
//         ntarg_(c) = 20,
//         ntarg_(d) = 20.32);

//     // Create an index sequence for the argument pack
//     constexpr auto indices = std::index_sequence_for<Args...>{};

//     // Create the integer sequence of indices using fold expression
//     constexpr auto sequence = create_index_sequence_helper<decltype(arg_holder), Args...>(indices);
//     static_assert(has_duplicates(sequence) == false, "Error: mismatch of arguments with positioned and non-positioned arguments");
//     constexpr auto arg_sequence = decltype(arg_holder)::make_index_sequence();
//     static_assert(holds_all_required<decltype(arg_holder)>(sequence, arg_sequence),
//                    "Positional arguments without default value have been not filled in");

//     auto tup =  _nt_get_function_arg_tuple_(arg_sequence, sequence, arg_holder, std::forward<Args>(args)...);
//     std::function<void(double, int, int, double)> func = static_cast<void(*)(double, int, int, double)>(foo_impl);
//     std::apply(func, tup);
    
// }

// template<typename T>
// constexpr void handle_equal_op(T&& a){
//     if constexpr (__NT_IsEqualReflectOp__V<T>){
//         constexpr T obj(a);
//         constexpr typename T::Type&& val = std::forward<typename T::Type>(a.val);
//     }
// }


// template<typename Arg1>
// inline constexpr __NT_arg_holder__<Arg1> make_arg_holder(Arg1&& arg1){
//     return __NT_arg_holder__<Arg1>(std::forward<Arg1>(arg1));
// }

// template<typename Arg1, typename Arg2>
// inline constexpr __NT_arg_holder__<Arg1, Arg2> make_arg_holder(Arg1&& arg1, Arg2&& arg2){
//     return __NT_arg_holder__<Arg1>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
// }

// template<typename Arg1, typename Arg2, typename Arg3>
// inline constexpr __NT_arg_holder__<Arg1, Arg2, Arg3> make_arg_holder(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3){
//     return __NT_arg_holder__<Arg1>(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3));
// }
// template<typename... Args>
// void foo(Args&&... args){
    
// }


#endif

