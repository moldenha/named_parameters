#include <iostream>
#include <utility>
#include "named_parameters.hpp"
#include "src/initializer_list.hpp"
//g++ -std=c++17 main.cpp -o main


namespace non_overloaded_functions{
void foo(int a, int b, int c, double d){
    std::cout << "adding them all up is "<<
       static_cast<double>(a+b+c)+d<<std::endl;
}

}

namespace overloaded_functions{

void foo(int a, int b, int c, double v){std::cout << "overload1 a is "<<a<<std::endl;}
void foo(float a, int b, int c, double v){std::cout << "overload2"<<std::endl;}
template<typename T>
T& foo(T& a, float b, int c, double v){std::cout << "overload3" << std::endl; a += static_cast<T>(b); return a;}

}


NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo_n)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = 3,
    ntarg_(d) = 3.4
NT_FINISH_NAMED_PARAMETER_FUNCTION_(non_overloaded_functions::foo)


NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo_o)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = 20,
    ntarg_(d) = 20.32
NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(overloaded_functions::foo)


class ExampleClass {
    int a, b, c;
    double d;

public:
    NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_(ExampleClass,
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(a, b, c, d),
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_(20, 20.32),
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_(int, int, int, double))
    : a(a), b(b), c(c), d(d) {}

    void print() {
        std::cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << std::endl;
    }
};

class ExampleClass2 {
    int a, b, c;
    double d;

public:
    NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_(ExampleClass2,
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(a, b, c, d),
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_(),
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_(int, int, int, double))
    : a(a), b(b), c(c), d(d) {}

    void print() {
        std::cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << std::endl;
    }
};




std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec){
    os << '{';
    for(const auto& val : vec)
        os << val << ' ';
    return os << '}';
}

namespace nt_funcs{
    int relu(const int& val){return val;}
    float relu(const float& val){return val;}
    float relu(std::initializer_list<double> val){return float(*val.begin());}
    
    int il_test(int& a, std::vector<int> b, bool c, std::vector<int> d, std::vector<int> e, int f){a = 10; return 0;}
    int il_test(float a, std::vector<int> b, bool c, std::vector<int> d, std::vector<int> e, int f){std::cout << "vector d is "<<d<<std::endl; std::cout << "and b is "<<b<<std::endl;return 0;}

int sil_test(int a, std::vector<int> b){return 0;}    
}

NT_MAKE_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(il_test)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = false,
    ntarg_(d) = {3, 4},
    ntarg_(e) = {6, 7},
    ntarg_(f) = 25
NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(il_test, 6, nt_funcs::il_test)

NT_MAKE_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(sil_test)
    ntarg_(a),
    ntarg_(b) = {1, 2}
 NT_FINISH_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(sil_test, 2, nt_funcs::sil_test)



//testing nt_explicit_:

class initializer_taker{
    int a;
    public:
    initializer_taker(std::initializer_list<int> il)
    :a(*il.begin())
    {;}
    initializer_taker()
    :a(0)
    {;}

};


// Helper to combine multiple lambdas
template<class... Ts>
struct nt_overload : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
nt_overload(Ts...) -> nt_overload<Ts...>;


//template<typename T, T... idxs>
//inline constexpr auto get_string_lit(std::integer_sequence<T, idxs...>) {
//    constexpr auto literal = "variable_one"_literal_function_argumentntarg;

//    return nt::named_arguments::NT_StringLiteral_<
//        literal.char_at(idxs)...
//    >{};
//}


//// Helper: Expands an integer_sequence into a lambda
//template<typename F, typename T, T... Is>
//inline constexpr auto expand_sequence(F&& f, std::integer_sequence<T, Is...>) {
//    return f(std::integral_constant<T, Is>{}...);
//}

//template<char... Str>
//void print_nt_string_liter(nt::named_arguments::NT_StringLiteral_<Str...>){
//    ((std::cout << Str), ...);  // fold expression over comma
//}

//template<std::size_t N>
//inline constexpr std::size_t string_literal_size(const char (&)[N]) {
//    return N - 1;  // exclude null terminator
//}

////this is to try and test the making of the NT_StringLteral:
//void n_string_literal_test(){
//    // using nt::named_parameters
//    constexpr size_t size = ("variable_one"_literal_function_argumentntarg).var_name_size();

//    std::cout << "size is: "<<size<<std::endl;
//    auto literal = get_string_lit(std::make_integer_sequence<std::size_t, size>{});
    
//    auto lambda = [](auto... idxs) {
//        constexpr auto literal = "variable_one"_literal_function_argumentntarg;
//        return nt::named_arguments::NT_StringLiteral_<
//            literal.char_at(idxs)...
//        >{};
//    };
//    constexpr auto seq = std::make_integer_sequence<std::size_t, size>{};
    
//    auto lambda_literal = expand_sequence([](auto... idxs){
//        constexpr auto literal = "variable_one"_literal_function_argumentntarg;
//        return nt::named_arguments::NT_StringLiteral_<literal.char_at(idxs)...>{};
//    }, std::make_integer_sequence<std::size_t, string_literal_size("variable_one")>{});
//    print_nt_string_liter(lambda_literal);
//}



int main(){

    // n_string_literal_test();

    double a = 3.1415;
    foo_n(ntarg_(b) = 10, ntarg_(a) = a);
    foo_n(a, ntarg_(c) = 30, ntarg_(b) = 45);

    foo_o(ntarg_(b) = 3.146, ntarg_(a) = a) += 10;
    std::cout << "a is now "<<a<<std::endl;
    foo_o(static_cast<int>(a),2,ntarg_(d)=a);

    ExampleClass cls(ntarg_(b) = 10.2, ntarg_(c) = 2, ntarg_(a) = 1, ntarg_(d) = 3.4);
    cls.print();
    
    ExampleClass2 cls2(ntarg_(b) = 10.2, ntarg_(c) = 2, ntarg_(a) = 1, ntarg_(d) = 3.4);
    cls2.print();
    
    int holding_for_il = 1;

    il_test(holding_for_il, {1, 2, 3}, ntarg_(d) = {4, 4, 5, 5, 6});
    std::cout << "holding for il is now "<<holding_for_il << std::endl;
    
    sil_test(1, {3, 2, 3});
    return 0;


}


