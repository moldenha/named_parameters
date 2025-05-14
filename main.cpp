#include <iostream>
#include <utility>
#include "named_parameters.hpp"
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


_NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo_n)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = 3,
    ntarg_(d) = 3.4
_NT_FINISH_NAMED_PARAMETER_FUNCTION_(non_overloaded_functions::foo)


_NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo_o)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = 20,
    ntarg_(d) = 20.32
_NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(overloaded_functions::foo,
                                    _NT_OVERLOAD_PARAMETERS_(void, int, int, int, double),
                                    _NT_OVERLOAD_PARAMETERS_(void, float, int, int, double),
                                    _NT_OVERLOAD_PARAMETERS_(::nt::named_arguments::_nt_auto_return_<0>&, ::nt::named_arguments::_nt_auto_&, ::nt::named_arguments::_nt_convertible_<float>, int, double))


class ExampleClass {
    int a, b, c;
    double d;

public:
    _NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_(ExampleClass,
        _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(a, b, c, d),
        _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_(20, 20.32),
        _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_(int, int, int, double))
    : a(a), b(b), c(c), d(d) {}

    void print() {
        std::cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << std::endl;
    }
};




int main(){


    double a = 3.1415;
    foo_n(ntarg_(b) = 10, ntarg_(a) = a);
    foo_n(a, ntarg_(c) = 30, ntarg_(b) = 45);

    foo_o(ntarg_(b) = 3.146, ntarg_(a) = a) += 10;
    std::cout << "a is now "<<a<<std::endl;
    foo_o(static_cast<int>(a),2,ntarg_(d)=a);

    ExampleClass cls(ntarg_(b) = 10.2, ntarg_(c) = 2, ntarg_(a) = 1, ntarg_(d) = 3.4);
    cls.print();
    
    return 0;
}


