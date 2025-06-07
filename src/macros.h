#ifndef __NT_NAMED_ARGUMENTS_ALL_GENERAL_REFLECT_MACROS_H__
#define __NT_NAMED_ARGUMENTS_ALL_GENERAL_REFLECT_MACROS_H__
#include "underscore_op.hpp"
#include "function_wrap.hpp"
#include "overload_op.hpp"
#include "expanded_class_constructor_macros.h"
#include "class_constructor.hpp"

#define _NT_GET_EQUAL_REFLECT_OP_0_(str) str.char_at(0), _NT_GET_EQUAL_REFLECT_OP_1_(str)
#define _NT_GET_EQUAL_REFLECT_OP_1_(str) str.char_at(1), _NT_GET_EQUAL_REFLECT_OP_2_(str)
#define _NT_GET_EQUAL_REFLECT_OP_2_(str) str.char_at(2), _NT_GET_EQUAL_REFLECT_OP_3_(str)
#define _NT_GET_EQUAL_REFLECT_OP_3_(str) str.char_at(3), _NT_GET_EQUAL_REFLECT_OP_4_(str)
#define _NT_GET_EQUAL_REFLECT_OP_4_(str) str.char_at(4), _NT_GET_EQUAL_REFLECT_OP_5_(str)
#define _NT_GET_EQUAL_REFLECT_OP_5_(str) str.char_at(5), _NT_GET_EQUAL_REFLECT_OP_6_(str)
#define _NT_GET_EQUAL_REFLECT_OP_6_(str) str.char_at(6), _NT_GET_EQUAL_REFLECT_OP_7_(str)
#define _NT_GET_EQUAL_REFLECT_OP_7_(str) str.char_at(7), _NT_GET_EQUAL_REFLECT_OP_8_(str)
#define _NT_GET_EQUAL_REFLECT_OP_8_(str) str.char_at(8), _NT_GET_EQUAL_REFLECT_OP_9_(str)
#define _NT_GET_EQUAL_REFLECT_OP_9_(str) str.char_at(9), _NT_GET_EQUAL_REFLECT_OP_10_(str)
#define _NT_GET_EQUAL_REFLECT_OP_10_(str) str.char_at(10), _NT_GET_EQUAL_REFLECT_OP_11_(str)
#define _NT_GET_EQUAL_REFLECT_OP_11_(str) str.char_at(11), _NT_GET_EQUAL_REFLECT_OP_12_(str)
#define _NT_GET_EQUAL_REFLECT_OP_12_(str) str.char_at(12), _NT_GET_EQUAL_REFLECT_OP_13_(str)
#define _NT_GET_EQUAL_REFLECT_OP_13_(str) str.char_at(13), _NT_GET_EQUAL_REFLECT_OP_14_(str)
#define _NT_GET_EQUAL_REFLECT_OP_14_(str) str.char_at(14), _NT_GET_EQUAL_REFLECT_OP_15_(str)
#define _NT_GET_EQUAL_REFLECT_OP_15_(str) str.char_at(15), _NT_GET_EQUAL_REFLECT_OP_16_(str)
#define _NT_GET_EQUAL_REFLECT_OP_16_(str) str.char_at(16), _NT_GET_EQUAL_REFLECT_OP_17_(str)
#define _NT_GET_EQUAL_REFLECT_OP_17_(str) str.char_at(17), _NT_GET_EQUAL_REFLECT_OP_18_(str)
#define _NT_GET_EQUAL_REFLECT_OP_18_(str) str.char_at(18), _NT_GET_EQUAL_REFLECT_OP_19_(str)
#define _NT_GET_EQUAL_REFLECT_OP_19_(str) str.char_at(19), _NT_GET_EQUAL_REFLECT_OP_20_(str)
#define _NT_GET_EQUAL_REFLECT_OP_20_(str) str.char_at(20), _NT_GET_EQUAL_REFLECT_OP_21_(str)
#define _NT_GET_EQUAL_REFLECT_OP_21_(str) str.char_at(21), _NT_GET_EQUAL_REFLECT_OP_22_(str) 
#define _NT_GET_EQUAL_REFLECT_OP_22_(str) str.char_at(22)


#define _NT_GET_EQUAL_REFLECT_SUB_OP_TO_LITERAL_(str) ::nt::named_arguments::_NT_StringLiteral_<_NT_GET_EQUAL_REFLECT_OP_0_(str)>{} 
#define _NT_GET_EQUAL_REFLECT_SUB_OP_TO_NON_SUB_(type, str, var) ::nt::named_arguments::__NT_EqualReflectOp__<type, _NT_GET_EQUAL_REFLECT_OP_0_(str)>{var}

#define _NT_NAMED_ARGUMENTS_CONCAT_(a, b) a##b
#define ntarg_(arg) _NT_GET_EQUAL_REFLECT_SUB_OP_TO_LITERAL_(_NT_NAMED_ARGUMENTS_CONCAT_(#arg, _literal_function_argument_ntarg))
// #define ntdefault_arg_(arg, val) _NT_GET_EQUAL_REFLECT_SUB_OP_TO_NON_SUB_(decltype(val), _NT_GLUE_(#arg, _ntarg), val)

// Macro to start defining a function with named parameters
#define _NT_MAKE_NAMED_PARAMETER_FUNCTION_(func_name) \
template<typename... Args>\
inline decltype(auto) func_name(Args&&... args) {\
    static_assert(::nt::named_arguments::_nt_incoming_func_args_are_well_formed_<Args...>::value, \
                  "Positional arguments must all come before named ones, and all arguments must be defined");\
    auto arg_holder = ::nt::named_arguments::make_arg_holder(\

// Macro to finish defining a function and handle overload resolution
#define _NT_FINISH_NAMED_PARAMETER_FUNCTION_(implement_name) \
    );\
    constexpr auto indices = std::index_sequence_for<Args...>{};\
    constexpr auto sequence = ::nt::named_arguments::create_index_sequence_helper<decltype(arg_holder), Args...>(indices);\
    static_assert(::nt::named_arguments::has_duplicates(sequence) == false, "Error: mismatch of arguments with positioned and non-positioned arguments");\
    constexpr auto arg_sequence = decltype(arg_holder)::make_index_sequence();\
    static_assert(::nt::named_arguments::holds_all_required<decltype(arg_holder)>(sequence, arg_sequence), \
                "Positional arguments without default value have been not filled in"); \
    auto tup =  ::nt::named_arguments::_nt_get_function_arg_tuple_(arg_sequence, sequence, arg_holder, std::forward<Args>(args)...); \
    return std::apply(implement_name, tup); \
}

#define _NT_OVERLOAD_PARAMETERS_(...) ::nt::named_arguments::_nt_overload_operator_<__VA_ARGS__>


// Macro to handle function overload based on the argument types
#define _NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(implement_name, ...) \
    );\
    constexpr auto indices = std::index_sequence_for<Args...>{};\
    constexpr auto sequence = ::nt::named_arguments::create_index_sequence_helper<decltype(arg_holder), Args...>(indices);\
    static_assert(::nt::named_arguments::has_duplicates(sequence) == false, "Error: mismatch of arguments with positioned and non-positioned arguments");\
    constexpr auto arg_sequence = decltype(arg_holder)::make_index_sequence();\
    static_assert(::nt::named_arguments::holds_all_required<decltype(arg_holder)>(sequence, arg_sequence), \
                "Positional arguments without default value have been not filled in"); \
    auto tup =  ::nt::named_arguments::_nt_get_function_arg_tuple_(arg_sequence, sequence, arg_holder, std::forward<Args>(args)...); \
    using overload_type = ::nt::named_arguments::_nt_multi_overload_operator_< __VA_ARGS__ >; \
    constexpr std::size_t callable_index = overload_type::template callable_index<decltype(tup)>();\
    if constexpr (callable_index < overload_type::Size) {\
        using function_overload_type = overload_type::overload_type<callable_index>; \
        using fixed_function_overload_type = typename function_overload_type::template fix_overload_type<decltype(tup)>; \
        typename fixed_function_overload_type::functional_type func = static_cast<typename fixed_function_overload_type::func_type>(implement_name); \
        return std::apply(func, tup); \
    } else {\
        constexpr std::size_t convertible_index = overload_type::template callable_convertible_index<decltype(tup)>(); \
        static_assert(convertible_index < overload_type::Size, \
                      "Error, no types convertible to be able to run overloaded function"); \
        using function_overload_type = overload_type::overload_type<callable_index>; \
        using fixed_function_overload_type = typename function_overload_type::template fix_overload_type<decltype(tup)>; \
        typename fixed_function_overload_type::functional_type func = static_cast<typename fixed_function_overload_type::func_type>(implement_name); \
        return std::apply(func, tup); \
    } \
}


#define _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(...) __VA_ARGS__
#define _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_(...) __VA_ARGS__
#define _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_(...) __VA_ARGS__

#define _NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_(ClassName, named_args, default_args, typed_args) \
    template<typename... Args> \
    ClassName(Args&&... args) : \
        ClassName(std::make_from_tuple<ClassName>( \
            ::nt::named_arguments::make_constructable_tuple( \
                ::nt::named_arguments::make_arg_holder(_NT_MAKE_NTARGS_MACRO_EXPAND_(named_args)).define_last_defaults(default_args), \
                _NT_OVERLOAD_PARAMETERS_(void, typed_args){}, \
                std::forward<Args>(args)...)) \
        ) {;} \
    ClassName(_NT_REFLECT_CONSTRUCTOR_SELECT_MACRO_(typed_args, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_62_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_61_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_60_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_59_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_58_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_57_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_56_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_55_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_54_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_53_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_52_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_51_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_50_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_49_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_48_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_47_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_46_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_45_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_44_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_43_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_42_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_41_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_40_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_39_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_38_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_37_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_36_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_35_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_34_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_33_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_32_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_31_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_30_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_29_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_28_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_27_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_26_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_25_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_24_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_23_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_22_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_21_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_20_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_19_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_18_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_17_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_16_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_15_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_14_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_13_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_12_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_11_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_10_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_9_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_8_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_7_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_6_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_5_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_4_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_3_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_2_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_1_, _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_0_)(typed_args, named_args))




#endif
