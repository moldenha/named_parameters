#ifndef NT_NAMED_ARGUMENT_PARAMETERS_UNDERSCORE_OP_HPP__
#define NT_NAMED_ARGUMENT_PARAMETERS_UNDERSCORE_OP_HPP__
#include <tuple>
#include <optional>
#include <type_traits>
#include <iostream>
#include <cstddef>
#include "value_holder.hpp"

namespace nt{
namespace named_arguments{


// This is a struct purely to hold stringed values
struct NT_EqualReflectOp_sub__{
    const char* var_name;  // points to the literal's characters (static storage)
    std::size_t sz;        // length of the literal (excluding null terminator)
    

    template<std::size_t N>
    constexpr NT_EqualReflectOp_sub__(const char(&a)[N]) : var_name(a), sz(N-1) {}
    
    // constexpr constructor to allow compile-time initialization
    constexpr NT_EqualReflectOp_sub__(const char* str, std::size_t len)
        : var_name(str), sz(len) { }


    inline constexpr std::size_t var_name_size() const { return sz; }


    // Example: constexpr function to get a character (for demo purposes)
    inline constexpr char char_at(std::size_t i) const {
        return i < sz ? var_name[i] : '\0';
    }
};





template<char... Strs>
struct NT_StringLiteral_;

template<typename T, char... Strs>
struct NT_EqualReflectOp__{
    using Type = T;
    using LiteralType = NT_StringLiteral_<Strs...>;
    value_holder<T> val;
    // T&& val;
    constexpr NT_EqualReflectOp__(T&& _val) : val(std::forward<T>(_val)) {;}
    inline static constexpr NT_StringLiteral_<Strs...> literal() {return NT_StringLiteral_<Strs...>{};}
    
    //static constexpr char* var_name() {return {Strs...};} 
    //the above causes an error, 
    //but it allows the class to be seen and is good for debugging

    template<typename G, char... Strs2>
    constexpr bool operator==(const NT_EqualReflectOp__<G, Strs...>&);
    constexpr NT_EqualReflectOp__(const NT_EqualReflectOp__& v)
    :val(std::forward<T>(const_cast<NT_EqualReflectOp__&>(v).val.get())){}

    inline constexpr NT_EqualReflectOp__& operator=(T&& _val) {
        val = value_holder(std::forward<T>(_val));
        return *this;  // Return the current object instead of creating a new one
    }
};



template<char c, char... Strs>
inline constexpr NT_StringLiteral_<Strs...> remove_char(){
    return {};
}

template<char c, char... Strs>
inline constexpr char get_first_char(){
    return c;
}


template<char... Strs>
struct NT_StringLiteral_ {
    static constexpr std::size_t Size = sizeof...(Strs);
    
    template<typename T>
    using equal_op_type = NT_EqualReflectOp__<T, Strs...>;

    template<std::size_t val>
    inline static constexpr char get_char(){
        static_assert(val < Size, "Getting char out of range");
        if constexpr (val == 0){
            return get_first_char<Strs...>();
        }else{
            constexpr auto res = remove_char<Strs...>();
            return decltype(res)::template get_char<val-1>();
        }
    }
    
    template<char... Strs2>
    inline constexpr bool operator==(NT_StringLiteral_<Strs2...>) const {
        if constexpr (sizeof...(Strs) != sizeof...(Strs2)){
            return false;
        }else{
            return ((Strs == Strs2) && ...);
        }
    }

    template<char... Strs2>
    inline static constexpr bool isEqual(NT_StringLiteral_<Strs2...>){
        if constexpr (sizeof...(Strs) != sizeof...(Strs2)){
            return false;
        }else{
            return ((Strs == Strs2) && ...);
        }
    }

    //static constexpr char* var_name() {return {Strs...};} 
    //the above causes an error, 
    //but it allows the class to be seen and is good for debugging

    template<typename T2>
    inline constexpr NT_EqualReflectOp__<T2, Strs...> operator=(T2&& val2) const& {
        return NT_EqualReflectOp__<T2, Strs...>(std::forward<T2>(val2));
    }
    
    template<typename T2>
    inline constexpr NT_EqualReflectOp__<std::initializer_list<T2>, Strs...> operator=(std::initializer_list<T2>&& ls) const& {
        return NT_EqualReflectOp__<std::initializer_list<T2>, Strs...>(std::forward<std::initializer_list<T2>>(ls));
    }

    template<char nChar>
    using add_char_type = NT_StringLiteral_<Strs..., nChar>;

};



template<typename T, char... Strs>
template<typename G, char... Strs2>
inline constexpr bool NT_EqualReflectOp__<T, Strs...>::operator==(const NT_EqualReflectOp__<G, Strs...>& other){
    return std::is_same_v<G, T> && this->literal() == other.literal();
}



template <typename T>
struct isNT_EqualReflectOp__ : std::false_type {};

template <typename T, char... Chars>
struct isNT_EqualReflectOp__<NT_EqualReflectOp__<T, Chars...>> : std::true_type {};

// Helper variable template to make it easier to check
template <typename T>
inline constexpr bool NT_IsEqualReflectOp__V = isNT_EqualReflectOp__<std::remove_cv_t<std::remove_reference_t<T>>>::value;

template<typename Arg, typename... Args>
struct is_allNT_EqualReflectOp__{
    static constexpr bool value = NT_IsEqualReflectOp__V<Arg> && is_allNT_EqualReflectOp__<Args...>::value; 
};

template<typename Arg>
struct is_allNT_EqualReflectOp__<Arg>{
    static constexpr bool value = (NT_IsEqualReflectOp__V<Arg>);
};
template<>
struct is_allNT_EqualReflectOp__<NT_StringLiteral_<'h'>>{
    static constexpr bool value = true;
};


template<typename T>
struct isnt_string_literal : std::false_type {};

template<char... Chars>
struct isnt_string_literal<NT_StringLiteral_<Chars...> > : std::true_type{};

template<typename T>
inline constexpr bool isnt_string_literal_v = isnt_string_literal<std::remove_cv_t<std::remove_reference_t<T>>>::value; 



template<typename Arg, typename... Args>
struct nt_all_string_literals_{
    static constexpr bool value = (isnt_string_literal_v<Arg> || NT_IsEqualReflectOp__V<Arg>) && nt_all_string_literals_<Args...>::value; 
};

template<typename Arg>
struct nt_all_string_literals_<Arg>{
    static constexpr bool value = (isnt_string_literal_v<Arg> || NT_IsEqualReflectOp__V<Arg>);
};
template<>
struct nt_all_string_literals_<NT_StringLiteral_<'h'>>{
    static constexpr bool value = true;
};

}}
//::nt::named_arguments


//this is a namespace containing all of the functions needed to make a one-line constexpr literal string
//of any size that is cross-platform
namespace nt::named_arguments::expand_details{
// Helper: Expands an integer_sequence into a lambda
template<typename F, typename T, T... Is>
inline constexpr auto expand_sequence_to_lambda(F&& f, std::integer_sequence<T, Is...>) {
    return f(std::integral_constant<T, Is>{}...);
}

template<std::size_t N>
inline constexpr std::size_t string_literal_size(const char (&)[N]) {
    return N - 1;  // exclude null terminator
}

}
//::nt::named_arguments::expand_details


// User-defined literal operator (works for any length string literal)
constexpr auto operator""_literal_function_argumentntarg(const char* __str, std::size_t __len) {
    return ::nt::named_arguments::NT_EqualReflectOp_sub__{__str, __len};
}

#endif
