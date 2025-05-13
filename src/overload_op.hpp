#ifndef __NT_NAMED_ARGUMENT_PARAMETERS_OVERLOAD_OP_HPP__
#define __NT_NAMED_ARGUMENT_PARAMETERS_OVERLOAD_OP_HPP__

#include <tuple>
#include <type_traits>
#include "tuple_funcs.hpp"
#include "type_traits.h"



namespace nt{
namespace named_arguments{
//used to mark a template that can take anything
struct _nt_auto_{};

template<std::size_t N>
struct _nt_auto_return_{static constexpr std::size_t Index = N;};

template<typename T> struct _is_nt_auto_return_ : std::false_type{};
template<std::size_t N> struct _is_nt_auto_return_<_nt_auto_return_<N>> : std::true_type{};
template<typename OutArg, typename Tuple>
struct _nt_extract_auto_return_type_{using type = OutArg;};
template<std::size_t N, typename Tuple>
struct _nt_extract_auto_return_type_<_nt_auto_return_<N>, Tuple> {using type = type_details::remove_cvref_t<std::tuple_element_t<N, Tuple>>;};
template<std::size_t N, typename Tuple>
struct _nt_extract_auto_return_type_<_nt_auto_return_<N>&, Tuple> {using type = type_details::remove_cvref_t<std::tuple_element_t<N, Tuple>>&;};
template<std::size_t N, typename Tuple>
struct _nt_extract_auto_return_type_<const _nt_auto_return_<N>&, Tuple> {using type = const type_details::remove_cvref_t<std::tuple_element_t<N, Tuple>>&;};
template<std::size_t N, typename Tuple>
struct _nt_extract_auto_return_type_<const _nt_auto_return_<N>, Tuple> {using type = const type_details::remove_cvref_t<std::tuple_element_t<N, Tuple>>;};


//used to make a variable that is meant to take multiple variables [for example nt::Scalar]
template<typename T>
struct _nt_convertible_{
    using Type = T;

    template<typename G>
    inline static constexpr bool is_converible(){return std::is_convertible_v<G, T>;}
};

//checks if a type is an _nt_convertible_
template<typename T>
struct _is_nt_convertable_ : std::false_type {};

template<typename T>
struct _is_nt_convertable_<_nt_convertible_<T> > : std::true_type {};


template<typename Arg, typename TArg>
inline static constexpr bool _nt_is_arg_callable_value_ = 
    (std::is_same_v<Arg, TArg> 
    ||  std::is_same_v<Arg, _nt_auto_>
    ||  (std::is_reference_v<TArg> && std::is_reference_v<Arg> && 
        ((std::is_same_v<type_details::remove_cvref_t<Arg>, type_details::remove_cvref_t<TArg>>) || std::is_same_v<type_details::remove_cvref_t<Arg>, _nt_auto_>)
         && ((!std::is_const_v<Arg> && !std::is_const_v<TArg>) || std::is_const_v<Arg>))
    || (std::is_reference_v<Arg> && std::is_const_v<Arg>)
    || (std::is_reference_v<TArg> && !std::is_reference_v<Arg>)
    || _is_nt_convertable_<Arg>::value);


//checks if all the types from an overload are callable
template<typename Arg1, typename... Args>
struct _nt_is_callable_{
    
    template<typename Tuple>
    inline static constexpr bool value(){
        //_nt_remove_first_tuple_type_impl<Tuple> is just going to get rid of the first tuple type
        if constexpr (std::tuple_size_v<Tuple> == 0){return true;}
        else{
            using TArg1 = std::tuple_element_t<0, Tuple>;
            if constexpr (_nt_is_arg_callable_value_<Arg1, TArg1>){
                if constexpr (sizeof...(Args) == 0) return true;
                else return _nt_is_callable_<Args...>::template value<typename _nt_remove_first_tuple_type_impl<Tuple>::type>();
            }else{
                return false;
            }

        }
    }

    template<typename Tuple>
    inline static constexpr bool convertible_value(){
        if constexpr (std::tuple_size_v<Tuple> == 0){return true;}
        else{
            using TArg1 = std::tuple_element_t<0, Tuple>;
            if constexpr (std::is_convertible_v<TArg1, Arg1>){
                if constexpr (sizeof...(Args) == 0){return true;}
                else return _nt_is_callable_<Args...>::template convertible_value<typename _nt_remove_first_tuple_type_impl<Tuple>::type>();
            }else if constexpr (std::is_same_v<_nt_auto_, Arg1>){
                if constexpr (sizeof...(Args) == 0){return true;}
                else return _nt_is_callable_<Args...>::template convertible_value<typename _nt_remove_first_tuple_type_impl<Tuple>::type>();
            }
            else if constexpr (_is_nt_convertable_<Arg1>::value){
                if constexpr (sizeof...(Args) == 0) {return true;}
                else {return Arg1::template is_converible<TArg1>() && _nt_is_callable_<Args...>::template convertible_value<typename _nt_remove_first_tuple_type_impl<Tuple>::type>();}
            }
            else{
                return false;
            }
        }
    }

};

// template<typename T>
// struct is_tuple<T> : std::false_type{};

// template<typename... Args>
// struct is_tuple<std::tuple<Args...>> : std::true_type{};

//Tup1 is the new tuple of elements that were inputted into the function
//Tup2 is the old tuple of elements that were the original function definition
template<typename T1, typename T2>
struct _nt_const_volatile_ref_like_{
    using base_type = type_details::remove_cvref_t<T2>;
    using type = std::conditional_t<std::is_reference_v<T1>,
                    std::conditional_t<std::is_const_v<T1>,
                        std::conditional_t<std::is_volatile_v<T1>,
                            const volatile base_type&,
                            const base_type&>,
                    std::conditional_t<std::is_volatile_v<T1>,
                        volatile base_type&,
                        base_type&>>,
                std::conditional_t<std::is_const_v<T1>,
                    std::conditional_t<std::is_volatile_v<T2>,
                        volatile const base_type, const base_type>,
                    std::conditional_t<std::is_volatile_v<T2>, volatile base_type, base_type> > >;
};


template<typename T1, typename T2>
struct _nt_fix_singular_arg_holder_type_{
    using type = std::conditional_t<std::is_same_v<type_details::remove_cvref_t<T2>, _nt_auto_>, typename _nt_const_volatile_ref_like_<T2, T1>::type, T2>;
};

template<typename T1, typename CT>
struct _nt_fix_singular_arg_holder_type_<T1, _nt_convertible_<CT>>{
    using type = CT;
};


template<typename Tup1, typename Tup2, typename sequence = typename detail::make_integer_sequence_helper<std::size_t, 0, std::tuple_size_v<Tup1>>::type>
struct _nt_fix_arg_holder_;

template<typename Tup1, typename Tup2, std::size_t... Is>
struct _nt_fix_arg_holder_<Tup1, Tup2, std::index_sequence<Is...>>{
    static_assert(std::tuple_size_v<Tup1> == std::tuple_size_v<Tup2>, "Expected tuples to have the same size to fix arguments, please review implementation");
    using type = std::tuple<typename _nt_fix_singular_arg_holder_type_<std::tuple_element_t<Is, Tup1>, std::tuple_element_t<Is, Tup2>>::type...>;
};



// template<std::size_t i, typename Tup1, typename Tup2>
// inline constexpr auto _nt_fix_arg_holder_(){
//     if constexpr(std::tuple_size_v<Tup1> <= i || std::tuple_size_v<Tup2> <= i){
//         return Tup1{};
//     }
//     else{
//         using out_type = typename _nt_fix_singular_arg_holder_type_<std::tuple_element_t<i, Tup1>, std::tuple_element_t<i, Tup2>>::type;
//         if constexpr (std::is_same_v<out_type, std::tuple_element_t<i, Tup1>>){
//             return _nt_fix_arg_holder_<i+1, Tup1, Tup2>();
//         }
//         else{
//             using replace_type = _nt_replace_tuple_type_t<i, out_type, Tup1>;
//             return _nt_fix_arg_holder_<i+1, replace_type, Tup2>();
            
//         }
//         // if constexpr (_is_nt_convertable_<std::tuple_element_t<i, Tup2>>::value){
//         //     using replace_type = _nt_replace_tuple_type_t<i, typename std::tuple_element_t<i, Tup2>::Type, Tup1>;
//         //     return _nt_fix_arg_holder_<i+1, replace_type, Tup2>();
//         // }
//         // else if constexpr(!std::is_same_v<std::tuple_element_t<i, Tup1>, std::tuple_element_t<i, Tup2> >
//         //             && std::is_same_v<type_details::remove_cvref_t<std::tuple_element_t<i, Tup1>>, 
//         //                 type_details::remove_cvref_t<std::tuple_element_t<i, Tup2>>>){ //this handles general case references, const's, and volatiles
//         //     using replace_type = _nt_replace_tuple_type_t<i, std::tuple_element_t<i, Tup2>, Tup1>;
//         //     return _nt_fix_arg_holder_<i+1, replace_type, Tup2>(); 
//         // }
//         // else if constexpr(std::is_same_v<std::tuple_element_t<i, Tup2>, _nt_auto_&>){
//         //     static_assert(std::is_reference_v<std::tuple_element_t<i, Tup1>>
//         //                 && !std::is_const_v<std::tuple_element_t<i, Tup1>>,
//         //                   "Erorr, unable to convert type to non-const reference");
//         //     using absolute_type = type_details::remove_cvref_t<std::tuple_element_t<i, Tup1>>&;
//         //     using replace_type = _nt_replace_tuple_type_t<i, absolute_type, Tup1>;
//         //     return _nt_fix_arg_holder_<i+1, replace_type, Tup2>(); 
//         // }
//         // else if constexpr(std::is_same_v<std::tuple_element_t<i, Tup2>, const _nt_auto_&>){
//         //     using absolute_type = const type_details::remove_cvref_t<std::tuple_element_t<i, Tup1> >&;
//         //     static_assert(type_details::can_bind_to_const_reference_or_construct<
//         //             std::tuple_element_t<i, Tup1>,
//         //             absolute_type>(),
//         //                 "Cannot convert element into a constant reference");
//         //     using replace_type = _nt_replace_tuple_type_t<i, absolute_type, Tup1>;
//         //     return _nt_fix_arg_holder_<i+1, replace_type, Tup2>(); 
//         // }
//         // else if constexpr(std::is_same_v<std::tuple_element_t<i, Tup2>, const volatile _nt_auto_&>){
//         //     using absolute_type = const volatile type_details::remove_cvref_t<std::tuple_element_t<i, Tup1> >&;
//         //     static_assert(type_details::can_bind_to_const_reference_or_construct<
//         //             std::tuple_element_t<i, Tup1>,
//         //             absolute_type>(),
//         //                 "Cannot convert element into a constant reference");
//         //     using replace_type = _nt_replace_tuple_type_t<i, absolute_type, Tup1>;
//         //     return _nt_fix_arg_holder_<i+1, replace_type, Tup2>(); 
//         // }
//         // else if constexpr(std::is_same_v<std::tuple_element_t<i, Tup2>, volatile _nt_auto_&>){
//         //     static_assert(std::is_reference_v<std::tuple_element_t<i, Tup1>>
//         //                 && !std::is_const_v<std::tuple_element_t<i, Tup1>>,
//         //                   "Erorr, unable to convert type to non-const reference");
//         //     using absolute_type = volatile type_details::remove_cvref_t<std::tuple_element_t<i, Tup1>>&;
//         //     using replace_type = _nt_replace_tuple_type_t<i, absolute_type, Tup1>;
//         //     return _nt_fix_arg_holder_<i+1, replace_type, Tup2>(); 
//         // }
//         // else if constexpr(std::is_same_v<std::tuple_element_t<i, Tup2>, _nt_auto_&&>){
//         //     using absolute_type = type_details::remove_cvref_t<std::tuple_element_t<i, Tup1>>&&;
//         //     using replace_type = _nt_replace_tuple_type_t<i, absolute_type, Tup1>;
//         //     return _nt_fix_arg_holder_<i+1, replace_type, Tup2>(); 
//         // }
//         // else{
//         //     return _nt_fix_arg_holder_<i+1, Tup1, Tup2>();
//         // }
//     }
// }


template<typename... Args>
struct _has_nt_auto_ : std::false_type {};

template<typename Arg, typename... Args>
struct _has_nt_auto_ <Arg, Args...>{
    static constexpr bool value = std::is_same_v<Arg, _nt_auto_> || _has_nt_auto_<Args...>::value;
};


template<typename... Args>
struct _has_nt_convertible_ : std::false_type {};

template<typename Arg, typename... Args>
struct _has_nt_convertible_<Arg, Args...>{
    static constexpr bool value = _is_nt_convertable_<Arg>::value || _has_nt_convertible_<Args...>::value;
};


template<typename OutArg, typename... ParamArgs>
struct _nt_overload_operator_;

template<typename OutArg, typename Tuple>
struct _nt_overload_operator_from_tuple_ {using type = _nt_overload_operator_<OutArg>;};

template<typename OutArg, typename... Args>
struct _nt_overload_operator_from_tuple_<OutArg, std::tuple<Args...>> {using type = _nt_overload_operator_<OutArg, Args...>;};

template<typename OutArg, typename... ParamArgs>
struct _nt_overload_operator_{
    using OutputType = OutArg;
    using func_type = OutArg(*) (ParamArgs...);
    using functional_type = typename std::function<OutArg(ParamArgs...)>;
    using param_tuple_type = typename std::tuple<ParamArgs...>;
    

    //this checks to see if the types specifically match unless specified
    //for example, templates they do not have to match
    //but for example;
    //(int, _nt_convertible_<nt::Scalar>, _nt_auto_)
    //can take:
    //(int, [Scalar Value including float, double, etc.] , [Any Variable])
    //
    template<class Type>
    inline static constexpr bool is_callable(){
        static_assert(type_details::is_specialization_of<std::tuple, Type>::value, "Expected tuple to compare overload arguments");
        static_assert(sizeof...(ParamArgs) <= std::tuple_size_v<Type>,
                      "[POTENTIAL INTERNAL ERROR] input types cannot have less than there are parameters for overloading a function");
        return _nt_is_callable_<ParamArgs...>::template value<Type>();
    }

    //this checks to see if the function call is valid if the types are convertible
    //for example a function
    //void(int, int, int)
    //could technically take (float, int, double)
    //because floats and doubles can be converted to ints
    template<class Type>
    inline static constexpr bool is_convertible_callable(){
        static_assert(type_details::is_specialization_of<std::tuple, Type>::value, "Expected tuple to compare overload arguments");
        static_assert(sizeof...(ParamArgs) <= std::tuple_size_v<Type>,
                      "[POTENTIAL INTERNAL ERROR] input types cannot have less than there are parameters for overloading a function");
        return _nt_is_callable_<ParamArgs...>::template convertible_value<Type>();
    }
    
    template<class Tuple>
    using fix_overload_type = typename _nt_overload_operator_from_tuple_<typename _nt_extract_auto_return_type_<OutArg, Tuple>::type, typename _nt_fix_arg_holder_<Tuple, std::tuple<ParamArgs...>>::type>::type;


    static constexpr bool has_auto = _has_nt_auto_<ParamArgs...>::value;
    static constexpr bool has_convertible = _has_nt_convertible_<ParamArgs...>::value;
    
    
};

template<typename T>
struct _is_nt_overload_operator_ : std::false_type {};

template<typename... Args>
struct _is_nt_overload_operator_<_nt_overload_operator_<Args...>> : std::true_type {};

template<typename... Args>
struct _all_is_nt_overload_operator_ : std::true_type {};

template<typename Arg1, typename... Args>
struct _all_is_nt_overload_operator_<Arg1, Args...>{
    static constexpr bool value = _is_nt_overload_operator_<Arg1>::value && _all_is_nt_overload_operator_<Args...>::value; 
};


template<std::size_t i, std::size_t out, bool found, bool convertible, typename Tup, typename... Args>
inline constexpr std::size_t _nt_find_valid_overload_operator_(){
    if constexpr(i == sizeof...(Args)){return out;}
    else if constexpr (found){
        if constexpr (convertible){
            static_assert(!std::tuple_element_t<i, std::tuple<Args...>>::template is_convertible_callable<Tup>(),
                          "Error got ambiguous function calls");
            return _nt_find_valid_overload_operator_<i+1, out, found, convertible, Tup, Args...>();
        }else{
            //so currently, the way that it works if 2 functions would work
            //if one has templates, and the other does not, then the one that does not have templates is used
            //otherwise it calls an ambiguity error
            //
            //[TODO:]
            //the way it should work is by going element by element and deciding which function should be used based on that
            //implement a heirarchy of which function should be used, and if they rank the same, then there is an ambiguity error
            if constexpr (std::tuple_element_t<i, std::tuple<Args...>>::template is_callable<Tup>()){
                if constexpr (std::tuple_element_t<i, std::tuple<Args...>>::has_auto){
                    return _nt_find_valid_overload_operator_<i+1, out, found, convertible, Tup, Args...>(); //pass
                }
                else if constexpr (std::tuple_element_t<out, std::tuple<Args...>>::has_auto){
                    return _nt_find_valid_overload_operator_<i+1, i, found, convertible, Tup, Args...>(); //pass
                }else{
                    static_assert(!std::tuple_element_t<i, std::tuple<Args...>>::template is_callable<Tup>(),
                                  "Error got ambiguous function calls");
                }
            }else{
                return _nt_find_valid_overload_operator_<i+1, out, found, convertible, Tup, Args...>();
            }
        }
    }else if constexpr(convertible){
        if constexpr (std::tuple_element_t<i, std::tuple<Args...>>::template is_convertible_callable<Tup>()){
            return _nt_find_valid_overload_operator_<i+1, i, true, true, Tup, Args...>(); 
        }else{
            return _nt_find_valid_overload_operator_<i+1, out, found, convertible, Tup, Args...>();
        }
    }else{
        if constexpr (std::tuple_element_t<i, std::tuple<Args...>>::template is_callable<Tup>()){
            return _nt_find_valid_overload_operator_<i+1, i, true, false, Tup, Args...>(); 
        }else{
            return _nt_find_valid_overload_operator_<i+1, out, found, convertible, Tup, Args...>();
        }
    }
}

template<typename... Args>
struct _nt_multi_overload_operator_{
    static_assert(_all_is_nt_overload_operator_<Args...>::value,
                  "[INTERNAL LOGIC ERROR] Expected to parse multiple overload operators");

    static constexpr std::size_t Size = sizeof...(Args);
    
    template<typename Tup>
    inline static constexpr std::size_t callable_convertible_index(){return _nt_find_valid_overload_operator_<0, Size, false, true, Tup, Args...>();}
    template<typename Tup>
    inline static constexpr std::size_t callable_index(){return _nt_find_valid_overload_operator_<0, Size, false, false, Tup, Args...>();}

    template<std::size_t N>
    using overload_type = std::tuple_element_t<N, std::tuple<Args...>>;
};


// void foo_overload(int a, int b, int c, double v){std::cout << "overload1"<<std::endl;}
// void foo_overload(float a, int b, int c, double v){std::cout << "overload2"<<std::endl;}
// template<typename T>
// T& foo_overload(T& a, float b, int c, double v){std::cout << "overload3" << std::endl; a += 3.3; return a;}


// template<typename... Args>
// inline constexpr auto foo_overload_test(Args&&... args){
//     static_assert(_nt_incoming_func_args_are_well_formed_<Args...>::value, "Positional arguments must all come before named ones, and all arguments must be defined");
//     auto arg_holder = make_arg_holder(
//         ntarg_(a),
//         ntarg_(b),
//         ntarg_(c) = 20,
//         ntarg_(d) = 20.32);

//     // Create an index sequence for the argument pack
//     constexpr auto indices = std::index_sequence_for<Args...>{};

//     // Create the integer sequence of indices using fold expression
//     constexpr auto sequence = create_index_sequence_helper<decltype(arg_holder), Args...>(arg_holder, indices);
//     static_assert(has_duplicates(sequence) == false, "Error: mismatch of arguments with positioned and non-positioned arguments");
//     constexpr auto arg_sequence = decltype(arg_holder)::make_index_sequence();
//     static_assert(holds_all_required<decltype(arg_holder)>(sequence, arg_sequence),
//                    "Positional arguments without default value have been not filled in");

//     auto tup =  _nt_get_function_arg_tuple_(arg_sequence, sequence, arg_holder, std::forward<Args>(args)...);
//     using overload_type =
//         _nt_multi_overload_operator_<
//             _nt_overload_operator_<void, int, int, int, double>,
//             _nt_overload_operator_<void, float, int, int, double>,
//             _nt_overload_operator_<_nt_auto_return_<0>&, _nt_auto_&, _nt_convertible_<float>, int, double>
//         >;
//     constexpr std::size_t callable_index = overload_type::template callable_index<decltype(tup)>();
//     if constexpr (callable_index < overload_type::Size){
//         using function_overload_type = overload_type::overload_type<callable_index>;
//         using fixed_function_overload_type = typename function_overload_type::template fix_overload_type<decltype(tup)>;
//         typename fixed_function_overload_type::functional_type func = static_cast<typename fixed_function_overload_type::func_type>(foo_overload);
//         return std::apply(func, tup);
//     }else{
//         constexpr std::size_t convertible_index = overload_type::template callable_convertible_index<decltype(tup)>();
//         static_assert(convertible_index < overload_type::Size,
//                       "Error, no types convertible to be able to run overloaded function");
//         using function_overload_type = overload_type::overload_type<callable_index>;
//         using fixed_function_overload_type = typename function_overload_type::template fix_overload_type<decltype(tup)>;
//         typename fixed_function_overload_type::functional_type func = static_cast<typename fixed_function_overload_type::func_type>(foo_overload);
//         return std::apply(func, tup);
//     }
//     // std::function<void(double, int, int, double)> func = static_cast<void(*)(double, int, int, double)>(foo_impl);
//     // std::apply(func, tup);
    
// }

}}

#endif
