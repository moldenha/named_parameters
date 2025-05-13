#ifndef __NT_NAMED_ARGUMENT_PARAMETERS_UNDERSCORE_OP_HPP__
#define __NT_NAMED_ARGUMENT_PARAMETERS_UNDERSCORE_OP_HPP__
#include <tuple>
#include <optional>
#include <type_traits>
#include <iostream>
#include <cstddef>

namespace nt{
namespace named_arguments{


template<typename T, typename = void>
struct __NT_EqualReflectOp_sub__{
    using type = T;
    const char* var_name;
    std::size_t sz;
    T&& var;

    constexpr __NT_EqualReflectOp_sub__(const char* str, std::size_t len, T&& _var) : var_name(str), sz(len), var(std::forward<T>(_var)){}
    inline constexpr std::size_t var_name_size() const {return sz;}
    // template<std::size_t N>
    // inline constexpr char get_at() const {
    //     return N < sz ? var_name[N] : '\0';
    // }

    // Example: constexpr function to get a character (for demo purposes)
    inline constexpr char char_at(std::size_t i) const {
        return i < sz ? var_name[i] : '\0';
    }
};

template<typename T>
struct __NT_EqualReflectOp_sub__<T, typename std::enable_if<std::is_same_v<T, std::nullopt_t>>::type>{
    using type = std::nullopt_t;
    const char* var_name;  // points to the literal's characters (static storage)
    std::size_t sz;        // length of the literal (excluding null terminator)
    

    template<std::size_t N>
    constexpr __NT_EqualReflectOp_sub__(const char(&a)[N]) : var_name(a), sz(N-1) {}
    
    // constexpr constructor to allow compile-time initialization
    constexpr __NT_EqualReflectOp_sub__(const char* str, std::size_t len)
        : var_name(str), sz(len) { }


    inline constexpr std::size_t var_name_size() const { return sz; }


    template<typename G>
    inline constexpr __NT_EqualReflectOp_sub__<G> operator=(G&& element) const {
        return __NT_EqualReflectOp_sub__<G>(var_name, sz, std::forward<G>(element));
    }
    
    template<typename G>
    inline constexpr __NT_EqualReflectOp_sub__<G> operator*(G&& element) const {
        return __NT_EqualReflectOp_sub__<G>(var_name, sz, std::forward<G>(element));
    }

    // template<std::size_t N>
    // constexpr char get_at() const {
    //     return N < sz ? var_name[N] : '\0';
    // }

    // Example: constexpr function to get a character (for demo purposes)
    inline constexpr char char_at(std::size_t i) const {
        return i < sz ? var_name[i] : '\0';
    }
};





template<char... Strs>
struct _NT_StringLiteral_;

template<typename T, char... Strs>
struct __NT_EqualReflectOp__{
    using Type = T;
    using LiteralType = _NT_StringLiteral_<Strs...>;
    T&& val;
    constexpr __NT_EqualReflectOp__(T&& _val) : val(std::forward<T>(_val)) {;}
    inline static constexpr _NT_StringLiteral_<Strs...> literal() {return _NT_StringLiteral_<Strs...>{};}
    
    //static constexpr char* var_name() {return {Strs...};} 
    //the above causes an error, 
    //but it allows the class to be seen and is good for debugging

    template<typename G, char... Strs2>
    constexpr bool operator==(const __NT_EqualReflectOp__<G, Strs...>&);
    constexpr __NT_EqualReflectOp__(const __NT_EqualReflectOp__& v)
    :val(std::forward<T>(const_cast<__NT_EqualReflectOp__&>(v).val)){}

    inline constexpr __NT_EqualReflectOp__& operator=(T&& _val) {
        val = std::forward<T>(_val);
        return *this;  // Return the current object instead of creating a new one
    }
};



template<char c, char... Strs>
inline constexpr _NT_StringLiteral_<Strs...> remove_char(){
    return {};
}

template<char c, char... Strs>
inline constexpr char get_first_char(){
    return c;
}


template<char... Strs>
struct _NT_StringLiteral_ {
    static constexpr std::size_t Size = sizeof...(Strs);
    
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
    inline constexpr bool operator==(_NT_StringLiteral_<Strs2...>) const {
        if constexpr (sizeof...(Strs) != sizeof...(Strs2)){return false;}
        if constexpr (sizeof...(Strs) == 0) return true;
        else{
            constexpr char c_1 = get_first_char<Strs...>();
            constexpr char c_2 = get_first_char<Strs2...>();
            if constexpr (c_1 != c_2) return false;
            return remove_char<Strs...>() == remove_char<Strs2...>();
        }
    }

    template<char... Strs2>
    inline static constexpr bool isEqual(_NT_StringLiteral_<Strs2...>){
        if constexpr (sizeof...(Strs) != sizeof...(Strs2)){return false;}
        if constexpr (sizeof...(Strs) == 0) return true;
        else{
            constexpr char c_1 = get_first_char<Strs...>();
            constexpr char c_2 = get_first_char<Strs2...>();
            if constexpr (c_1 != c_2) return false;
            constexpr auto _na = remove_char<Strs...>();
            constexpr auto _nb = remove_char<Strs2...>();
            return decltype(_na)::isEqual(decltype(_nb){});
        } 
    }

    //static constexpr char* var_name() {return {Strs...};} 
    //the above causes an error, 
    //but it allows the class to be seen and is good for debugging

    template<typename T2>
    inline constexpr __NT_EqualReflectOp__<T2, Strs...> operator=(T2&& val2) {
        return __NT_EqualReflectOp__<T2, Strs...>(std::forward<T2>(val2));
    }

    template<char nChar>
    using add_char_type = _NT_StringLiteral_<Strs..., nChar>;

};



template<typename T, char... Strs>
template<typename G, char... Strs2>
inline constexpr bool __NT_EqualReflectOp__<T, Strs...>::operator==(const __NT_EqualReflectOp__<G, Strs...>& other){
    return std::is_same_v<G, T> && this->literal() == other.literal();
}



template <typename T>
struct is__NT_EqualReflectOp__ : std::false_type {};

template <typename T, char... Chars>
struct is__NT_EqualReflectOp__<__NT_EqualReflectOp__<T, Chars...>> : std::true_type {};

// Helper variable template to make it easier to check
template <typename T>
inline constexpr bool __NT_IsEqualReflectOp__V = is__NT_EqualReflectOp__<std::remove_cv_t<std::remove_reference_t<T>>>::value;

template<typename T>
struct is_nt_string_literal : std::false_type {};

template<char... Chars>
struct is_nt_string_literal<_NT_StringLiteral_<Chars...> > : std::true_type{};

template<typename T>
inline constexpr bool is_nt_string_literal_v = is_nt_string_literal<std::remove_cv_t<std::remove_reference_t<T>>>::value; 



template<typename Arg, typename... Args>
struct _nt_all_string_literals_{
    static constexpr bool value = (is_nt_string_literal_v<Arg> || __NT_IsEqualReflectOp__V<Arg>) && _nt_all_string_literals_<Args...>::value; 
};

template<typename Arg>
struct _nt_all_string_literals_<Arg>{
    static constexpr bool value = (is_nt_string_literal_v<Arg> || __NT_IsEqualReflectOp__V<Arg>);
};
template<>
struct _nt_all_string_literals_<_NT_StringLiteral_<'h'>>{
    static constexpr bool value = true;
};

}}
//::nt::named_arguments

// User-defined literal operator (works for any length string literal)
constexpr auto operator""_literal_function_argument_ntarg(const char* __str, std::size_t __len) {
    return ::nt::named_arguments::__NT_EqualReflectOp_sub__<std::nullopt_t>{__str, __len};
}

#endif
