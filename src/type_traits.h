#ifndef NT_NAMED_ARGUMENT_PARAMETERS_SPECIFIC_TYPE_TRAITS_H__
#define NT_NAMED_ARGUMENT_PARAMETERS_SPECIFIC_TYPE_TRAITS_H__



namespace nt{
namespace named_arguments{
namespace type_details{

//important for finding if a type is a tuple by running is_specialization_of<std::tuple, Type>::value
template < template <typename...> class Template, typename T >
struct is_specialization_of : std::false_type {};

template < template <typename...> class Template, typename... Args >
struct is_specialization_of< Template, Template<Args...> > : std::true_type {};




template<typename T>
using remove_cvref_t = typename std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T, typename Arg>
inline constexpr bool can_bind_to_const_reference_or_construct() {
    if constexpr (std::is_lvalue_reference_v<Arg>) {
        // If Arg is an lvalue reference, check if it can bind to const T&
        return std::is_convertible_v<std::remove_reference_t<Arg>, T>;
    } else if constexpr (std::is_rvalue_reference_v<Arg>) {
        // If Arg is an rvalue reference, we can always bind it to const T&
        return true;
    } else {
        // For non-reference types, check if we can construct T from Arg
        return std::is_constructible_v<T, Arg>;
    }
}

}
}
}

#endif
