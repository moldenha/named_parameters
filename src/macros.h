#ifndef NT_NAMED_ARGUMENTS_ALL_GENERAL_REFLECT_MACROS_H__
#define NT_NAMED_ARGUMENTS_ALL_GENERAL_REFLECT_MACROS_H__
#include "underscore_op.hpp"
#include "function_wrap.hpp"
#include "expanded_class_constructor_macros.h"
#include "class_constructor.hpp"
#include "initializer_list.hpp"


#define NT_NAMED_ARGUMENTS_CONCAT_(a, b) a##b
#define ntarg_(arg) ::nt::named_arguments::expand_details::expand_sequence_to_lambda([](auto... idxs){\
                                constexpr std::string_view literal(#arg);\
                                return nt::named_arguments::NT_StringLiteral_<literal[idxs]...>{};\
                                }, std::make_integer_sequence<std::size_t, ::nt::named_arguments::expand_details::string_literal_size(#arg)>{})

// #define ntdefault_arg_(arg, val) NT_GET_EQUAL_REFLECT_SUB_OP_TO_NON_SUB_(decltype(val), NT_GLUE_(#arg, ntarg), val)

// Macro to start defining a function with named parameters
#define NT_MAKE_NAMED_PARAMETER_FUNCTION_(func_name) \
template<typename... Args>\
inline decltype(auto) func_name(Args&&... args) {\
    if constexpr (sizeof...(args) >= 2){\
        static_assert(::nt::named_arguments::nt_incoming_func_args_are_well_formed_<Args...>::value, \
                      "Positional arguments must all come before named ones, and all arguments must be defined");\
    }else if constexpr (sizeof...(args) == 1){\
        static_assert(!::nt::named_arguments::nt_all_string_literals_<Args...>::value\
                     || ::nt::named_arguments::is_allNT_EqualReflectOp__<Args...>::value\
                    , "All positional arguments must be defined");\
    }\
    auto arg_holder = ::nt::named_arguments::make_arg_holder(\

// Macro to finish defining a function and handle overload resolution
#define NT_FINISH_NAMED_PARAMETER_FUNCTION_(implement_name) \
    );\
    constexpr auto indices = std::index_sequence_for<Args...>{};\
    constexpr auto sequence = ::nt::named_arguments::create_index_sequence_helper<decltype(arg_holder), Args...>(indices);\
    static_assert(::nt::named_arguments::has_duplicates(sequence) == false, "Error: mismatch of arguments with positioned and non-positioned arguments");\
    constexpr auto arg_sequence = decltype(arg_holder)::make_index_sequence();\
    static_assert(::nt::named_arguments::holds_all_required<decltype(arg_holder)>(sequence, arg_sequence), \
                "Positional arguments without default value have been not filled in"); \
    auto tup =  ::nt::named_arguments::nt_get_function_arg_tuple_(arg_sequence, sequence, arg_holder, std::forward<Args>(args)...); \
    return std::apply(implement_name, tup); \
}

#define NT_OVERLOAD_PARAMETERS_(...) ::nt::named_arguments::nt_overload_operator_<__VA_ARGS__>


// Macro to handle function overload based on the argument types
#define NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(implement_name) \
        );\
    constexpr auto indices = std::index_sequence_for<Args...>{};\
    constexpr auto sequence = ::nt::named_arguments::create_index_sequence_helper<decltype(arg_holder), Args...>(indices);\
    static_assert(::nt::named_arguments::has_duplicates(sequence) == false, "Error: mismatch of arguments with positioned and non-positioned arguments");\
    constexpr auto arg_sequence = decltype(arg_holder)::make_index_sequence();\
    static_assert(::nt::named_arguments::holds_all_required<decltype(arg_holder)>(sequence, arg_sequence), \
                "Positional arguments without default value have been not filled in"); \
    auto tup =  ::nt::named_arguments::nt_get_function_arg_tuple_(arg_sequence, sequence, arg_holder, std::forward<Args>(args)...); \
    auto _nt_named_parameter_func_inner = [](auto&&... args) -> decltype(auto) {return implement_name(std::forward<decltype(args)>(args)...);};\
    return std::apply(_nt_named_parameter_func_inner, tup);\
}


//C++ by default does not handle initializer lists inside of function calls, therefore this has to be made
//This is a way to have a function by default allow initializer lists as function parameters
#define NT_MAKE_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(func_name)\
    namespace func_name##_reflect_il_details{\
        NT_MAKE_NAMED_PARAMETER_FUNCTION_(func_name##_il_able)

#define NT_FINISH_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(func_name, num_args, implement_name) \
    );\
    constexpr auto indices = std::index_sequence_for<Args...>{};\
    constexpr auto sequence = ::nt::named_arguments::create_index_sequence_helper<decltype(arg_holder), Args...>(indices);\
    static_assert(::nt::named_arguments::has_duplicates(sequence) == false, "Error: mismatch of arguments with positioned and non-positioned arguments");\
    constexpr auto arg_sequence = decltype(arg_holder)::make_index_sequence();\
    static_assert(::nt::named_arguments::holds_all_required<decltype(arg_holder)>(sequence, arg_sequence), \
                "Positional arguments without default value have been not filled in"); \
    using out_tup_type = ::nt::named_arguments::nt_function_traits<decltype(&implement_name)>::args_tuple;\
    auto tup =  ::nt::named_arguments::nt_get_supplied_function_arg_tuple_<out_tup_type>(arg_sequence, sequence, arg_holder, std::forward<Args>(args)...); \
    return std::apply(implement_name, tup); \
}\
}\
NT_REFLECT_NAMED_INITIALIZER_LIST_FUNCTION_##num_args(func_name, func_name##_reflect_il_details::func_name##_il_able)

#define NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(func_name, num_args, implement_name)\
    NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(implement_name)\
}\
NT_REFLECT_NAMED_INITIALIZER_LIST_FUNCTION_##num_args(func_name, func_name##_reflect_il_details::func_name##_il_able)


//Class Constructors
#define NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(...) __VA_ARGS__
#define NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_(...) __VA_ARGS__
#define NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_(...) __VA_ARGS__

#define NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_(ClassName, named_args, default_args, typed_args) \
    template<typename... Args> \
    ClassName(Args&&... args) : \
        ClassName(std::make_from_tuple<ClassName>( \
            ::nt::named_arguments::make_constructable_tuple( \
                ::nt::named_arguments::make_arg_holder(NT_MAKENTARGS_MACRO_EXPAND_(named_args)).define_last_defaults(default_args), \
                NT_OVERLOAD_PARAMETERS_(void, typed_args){}, \
                std::forward<Args>(args)...)) \
        ) {;} \
    ClassName(NT_REFLECT_CONSTRUCTOR_SELECT_MACRO_(typed_args, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_100_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_99_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_98_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_97_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_96_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_95_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_94_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_93_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_92_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_91_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_90_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_89_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_88_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_87_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_86_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_85_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_84_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_83_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_82_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_81_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_80_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_79_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_78_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_77_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_76_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_75_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_74_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_73_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_72_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_71_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_70_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_69_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_68_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_67_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_66_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_65_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_64_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_63_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_62_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_61_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_60_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_59_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_58_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_57_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_56_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_55_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_54_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_53_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_52_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_51_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_50_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_49_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_48_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_47_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_46_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_45_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_44_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_43_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_42_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_41_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_40_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_39_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_38_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_37_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_36_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_35_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_34_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_33_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_32_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_31_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_30_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_29_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_28_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_27_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_26_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_25_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_24_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_23_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_22_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_21_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_20_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_19_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_18_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_17_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_16_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_15_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_14_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_13_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_12_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_11_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_10_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_9_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_8_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_7_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_6_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_5_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_4_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_3_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_2_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_1_, NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_0_)(typed_args, named_args))




#endif
