# NeuroTensor Named Parameters

This is a named parameters library that was developed for [NeuroTensor](https://github.com/moldenha/NeuroTensor)

This library is cross platform C++17 compatible. The design was based around a few different needs for NeuroTensor having named parameters. This library deduces types, named types, and argument placement at compile time. It offers standard function named parameters, the ability to deduce which function should be used for an overload, and named parameters for class constructors. You will notice that most of the syntax has `nt` in the name, if you want to use the library outside of the intended NeuroTensor scope and do not want that syntax, you can go to the `macros.h` file and fairly easily change it.


## User Usage

The goal of this library is to have a fairly seamless use for users, and parameters can be used in any order. All that has to be included is `named_parameters.hpp`. Users may use functions as such:

```cpp
//foo.h
void foo(int a, int b, int c, double d);

//main.cpp
foo(3, ntarg_(d) = 3.4, ntarg_(c) = 5)

```

The setup to allow this to happen is fairly easy to implement from a developer standpoint.

```cpp
//foo.h

//it doesn't have to be in a namespace, it can be a different named function
namespace function_implementation{
void foo(int a, int b, int c, double d){
    std::cout << "adding them all up is "<<
       static_cast<double>(a+b+c)+d<<std::endl;
}

}

_NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo) //output function name for user to use
    ntarg_(a), //required argument
    ntarg_(b), //required argument
    ntarg_(c) = 3, //default argument
    ntarg_(d) = 3.4 //default argument
_NT_FINISH_NAMED_PARAMETER_FUNCTION_(function_implementation::foo) //the implementation of the function

//main.cpp

//user usage:
foo(3, ntarg_(d) = 3.4, ntarg_(c) = 5)
```

This named parameters library also allows for function overloads using `_NT_OVERLOAD_PARAMETERS_`, templates using `_nt_auto_`, deduction of return type based on templates using `_nt_auto_return_`, and explictly convertible parameters using `_nt_convertible_`. Below is an example using all of these:

```cpp
//foo.h

namespace overloaded_functions{

void foo(int a, int b, int c, double v){std::cout << "overload1 a is "<<a<<std::endl;}
void foo(float a, int b, int c, double v){std::cout << "overload2"<<std::endl;}
template<typename T>
T& foo(T& a, float b, int c, double v){std::cout << "overload3" << std::endl; a += static_cast<T>(b); return a;}

}

//developer implementation:
using namespace nt::named_arguments;

//notice that setup of named and default parameters are the same as with the non-overloaded function version
_NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = 20,
    ntarg_(d) = 20.32
_NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(overloaded_functions::foo,
                                    _NT_OVERLOAD_PARAMETERS_(void, int, int, int, double),
                                    _NT_OVERLOAD_PARAMETERS_(void, float, int, int, double),
                                    _NT_OVERLOAD_PARAMETERS_(_nt_auto_return_<0>&, _nt_auto_&, _nt_convertible_<float>, int, double))

//main.cpp

//all of the following are valid for a user to do, and the correct function will be deduced based on the parameters a user provides

double a = 3.1415;
foo(ntarg_(b) = 3.146, ntarg_(a) = a) += 10;
foo(static_cast<int>(a),2,ntarg_(d)=a);
```

The named parameters library also has a feature for class constructors to have named arguments:

```cpp
//MaxPool2D.h

//this is an example of the MaxPool2d class implementation in NeuroTensor:
class MaxPool2D{
    /* Stored Values */
    utils::my_n_tuple<2> kernel_size, stride, padding, dilation;
    bool return_indices, ceil_mode;
  public:
    
    _NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_(MaxPool2D,
        _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(kernel_size, stride, padding, dilation, return_indices, ceil_mode),
        _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_(-1, 0, 1, false, false), // everything but kernel_size has a default argument
        _NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_(utils::my_n_tuple<2>, utils::my_n_tuple<2>, utils::my_n_tuple<2>, utils::my_n_tuple<2>, bool, bool) )
    :kernel_size(kernel_size), stride(stride), padding(padding), dilation(dilation), return_indices(return_indices), ceil_mode(ceil_mode)
    {
        //notice that the constructor continues as normal
        if(this->stride == -1){this->stride = kernel_size;}
    }
    
    TensorGrad forward(TensorGrad) const;
    
};


//if you want to define the constrctor in another file:
//MaxPool2D.cpp

MaxPool2D::MaxPool2D(utils::my_n_tuple<2> kernel_size, utils::my_n_tuple<2> stride, utils::my_n_tuple<2> padding, utils::my_n_tuple<2> dilation, bool return_indices, bool ceil_mode)
//then continue with constructor as normal, the above is already defined in class

//main.cpp

//an example user usage:
MaxPool2D pool({2,2}, ntarg_(padding) = {1,0}, ntarg_(dilation) = 2, ntarg_(return_indices) = true);


```

## Features

Above shows general use cases. There are additional features and limitations outlined below. 

### Named Parameters Convention `ntarg_`

The use of `ntarg_` is pretty simple, it takes the variable name `ntarg_(a)` is the parameter name `a` and then it can be set to the value using `ntarg_(a) = 10`.

### Overloading Functions `_NT_OVERLOAD_PARAMETERS_`

When dealing with functions that have only one definition and are not overloaded, you can end the beggining construction that starts with `_NT_MAKE_NAMED_PARAMETER_FUNCTION_` that takes the argument of the outfunction name, with `_NT_FINISH_NAMED_PARAMETER_FUNCTION_` of where the function was originally defined. HoIver, when dealing with overloaded functions, you must end the definition with `_NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_`, folloId by the name of the original function, and then `_NT_OVERLOAD_PARAMETERS_` for each of the potential overloaded functions.

```cpp
//example:
_NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(overloaded_namespace::foo,
                            _NT_OVERLOAD_PARAMETERS_(void, int, int, int, int),
                            _NT_OVERLOAD_PARAMETERS_(int, double, double, double, double))

```

#### Using `_NT_OVERLOAD_PARAMETERS_`

`_NT_OVERLOAD_PARAMETERS_` takes the return type of the function folloId by all the types of the parameters of the function. For example, if you have a function defined as `int foo(double a, double b, double c, double d)` when you write the corresponding `_NT_OVERLOAD_PARAMETERS_` it would be `_NT_OVERLOAD_PARAMETERS_(int, double, double, double, double)`.

#### Limitations

Currently, each function overload must take the same number of arguments. This named parameters library cannot deduce the function using different number of parameters. 

The overloaded functions must also be named the same. In the future I would like to add an additional feature that can take multiple different functions and combine them into one. HoIver, currently, all functions must have the same name.

Deduction of the function to use is fairly limited. Currently, it looks for which overload the inputted parameters have the same types as, and then from there if none do, it looks which have variables that are all convertable. If multiple functions are deduced by either check, there will be an ambiguity error that is risen at compile time. I would like to have which function is to be used more efficiently, broadly, and applicably determined. If you would like to contribute and decide on a better way, look under `src/overload_op.hpp` in the `_nt_find_valid_overload_operator_` function.

### Argument Templates `_nt_auto_`

Templates are a vital part of C++, and not being able to have functions with template parameters would add some serious limitations to the library. When it comes to non-overloaded functions, they are just handled automatically and you do not have to worry about it. HoIver, when it comes to overloaded functions, and you have to explicitly write out each type, there is a way to handle templates built into the library using `_nt_auto_`.

#### Using `_nt_auto_` 

Take the two functions below:
```cpp
int foo(int, int, double)

template<typename T>
int foo(T&, int, double) //taking a reference to the template
```

Their overload(s) would be:
```cpp
_NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(foo,
                            _NT_OVERLOAD_PARAMETERS_(int, int, int, double),
                            _NT_OVERLOAD_PARAMETERS_(int, _nt_auto_&, int, double))
```

IMPORTANT: Notice how in the function, `foo(T&, int, double)`, `T&` is a reference, for that reason in `_NT_OVERLOAD_PARAMETERS_(int, _nt_auto_&, int, double)` the reference `_nt_auto_&` was used. The same follows for `const` or `volatile` expressions. It should be noted that `_nt_auto_` will basically just convert to anything as long as the aforementioned implications are kept in mind so `const SomeClass<T>&` as an argument would still be `const _nt_auto_&`

`_nt_auto_` can also be used for class constructors, just make sure to put `_nt_auto_` as one of the types.

### Return Templates `_nt_auto_return_`

Many times, functions in C++ will return template arguments. If you are not working with overloaded functions, you do not have to deal with this and the return type will be deduced for you. Otherwise you will have to use `_nt_auto_return_`.

#### Using `_nt_auto_return_`

`_nt_auto_return_` takes a `constexpr std::size_t` variable, that tells the library which parameter is type is going to be returned. 

For example:
```cpp
int foo(int, int, double)
template<typename T>
T& foo(T&, int, double) //taking a reference to the typename T, and returning a reference to the typename T
```

In the second `foo` overload, the typename `T` is retuend, and a reference to it is returned. Therefore, I know `T` is going to be defined as the first argument type, therefore the return type will be `_nt_auto_return_<0>`. I also know it is going to return a reference to the variable, therefore the return type will be `_nt_auto_return_<0>&`. This means the final overload will look like this:

```cpp
_NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(foo,
                            _NT_OVERLOAD_PARAMETERS_(int, int, int, double),
                            _NT_OVERLOAD_PARAMETERS_(_nt_auto_return_<0>&, _nt_auto_&, int, double))
```

#### Limitations

The deduction of the return type of functions are pretty limited. The type must be defined in one of the arguments. It also can only be deduced to be a referece, constant, or volatile to be returned, or a combination of the three. I would like to add ways to have the return type be defined using a combination of types and other structured binding types.


### Convertible Types `_nt_convertible_`

Some types are implictly meant to be converted into or constructed into. For example `nt::Scalar` is meant to take any scalar element and convert it to the correct type based on the `dtype` of any `nt::Tensor`. For that reason, when overloading a function, one of the noted types should be `_nt_convertible_<function_type>`. The library will check to see if it is valid by seeing if it is `std::is_convertible_v`.



### Class Construction `_NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_`

This is a way to define a class constructor that can take named parameter arguments. 
It defines two different constructors:
```cpp
template<typename... Args>
ClassName(Args&&...)

//and:
ClassName(/* all specified types and names */)
```


#### Using `_NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_`

`_NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_` takes four arguments, the first is the name of the class. The second is a `_NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_` which should be filled with the names of the arguments. The third is a `_NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_` which should be filled with any pertaining default values. The last is the `_NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_` which should have the types of each named arguments.

Using `_NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_` and `_NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_` happens in reverse. For example: `_NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(a, b, c)` and `_NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(20, 20.3)` will make `c` have the default argument `20.3` and `b` have the default argument `20` but `a` does not have a default argument.

#### Limitations

Using the macro `_NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_` does not have native support for template arguments in the class definition. Instead, you would have to do something like this:

```cpp

class MyClass{
    int a, b, c;
    public:
        template<typename... Args>
        MyClass(Args&&... args)
        :MyClass(std::make_from_tuple<MyClass>(
            ::nt::named_arguments::make_constructable_tuple(
                ::nt::named_arguments::make_arg_holder(
                    ntarg_(a), ntarg_(b) = 20, ntarg_(c) = 20.3
                ), _NT_OVERLOAD_PARAMETERS_(void, _nt_auto_, int, int){},
                std::forward<Args>(args)...))
            ) {;}
        
        template<typename T>
        MyClass(T a, int b, int c)
        :a(static_cast<int>(a)), b(b), c(c)
        {}
};

```

### Cross Compatibility

This library does not rely on the trick of using `template <typename CharT, CharT... Chars> constexpr _NT_StringLiteral_<Chars...> operator"" _ntarg()`. This is a `gcc` only extension for C++. This library aims to be cross compatible against all operating systems, while relying only on the standard C++ library. It is still able to deduce the name of each argument at compile time using `ntarg_`. HoIver, this does mean the use of macros, and therefore currently there is a character limit of 20 on the arguments. This can be changed in `src/macros.h`. 

The only thing that is evaluated at compile time are the inputted arguments and the default arguments are not exclusively `constexpr`. HoIver, placement of arguments, function overloading, class constructing, and everything that goes into that is evaluated at compile time.

## MSVC

This library can be compiled to work with `clang`, `gnu`, and `MSVC` for `C++`. When compiling on `MSVC` the option `/Zc:preprocessor` needs to be turned on.

### CMake

If compiling using `CMake`, the following can be added to your `CMakeLists.txt` file, and named parameters will compile without any issues, this is only so that it will work and stay cross-platform compatible when using `MSVC`:

```
if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()
```

## Contribution

I Ilcome contributions from the community to make this library even more poIrful and feature-rich! If you would like to contribute, here are some ways you can help:

1.  **Add New Features**:
    
    *   You can extend the library by adding new capabilities, such as better overload resolution, more flexible deduction of function return types, or improved support for template parameters in class constructors.
        
    *   If you have an idea for a new feature, feel free to propose it by opening an issue or submitting a pull request.
        
2.  **Reduce Limitations**:
    
    *   **Overload Resolution**: Currently, the library has some limitations in resolving function overloads, especially when there are multiple valid candidates. If you have ideas on how to make overload resolution more efficient and accurate, I'd love to hear them.
        
    *   **Class Constructors**: The class constructor feature works Ill for most cases, but if you encounter limitations, like handling more complex types or better support for template arguments, I encourage you to improve it.
        
    *   **Template Arguments Support**: I would like to expand the support for template arguments in class constructors and function overloads.
        
3.  **Improve Cross-Platform Compatibility**:
    
    *   While this library is designed to be cross-platform and C++17 compatible, there may be edge cases where certain environments or compilers behave differently. If you find any compatibility issues, please contribute a fix or improvement.
        
4.  **Performance Improvements**:
    
    *   As the library uses several macros and template metaprogramming techniques, there might be opportunities for performance optimizations, especially for large-scale use cases. If you have any performance optimization suggestions, feel free to share!
        

### How to Contribute:

1.  **Fork the Repository**: Create a personal fork of the repository on GitHub.
    
2.  **Clone the Repository**: Clone the forked repository to your local machine.
    
3.  **Make Changes**: Create a new branch for your changes, make your edits, and ensure your changes are Ill-documented.
    
4.  **Write Tests**: If your change introduces new functionality, please add tests to ensure it works as expected. I use \[insert your testing framework here\] for testing.
    
5.  **Submit a Pull Request**: After testing, submit a pull request to the main repository. Be sure to describe your changes clearly and provide a summary of any new features or fixes.
    

### Important Areas for Improvement:

*   **Function Overload Handling**: Currently, overload resolution is based on exact argument type matches. There’s potential to improve how I deduce the best overload, especially with complex or convertible types.
    
*   **Support for More Complex Types**: I are actively looking for ways to handle more complex types in both class constructors and overload resolution, such as supporting template arguments in constructors more efficiently, and for return types.
    
*   **Documentation & Examples**: If you have suggestions for improving the documentation or want to add more example use cases, feel free to contribute!
    

### Reporting Issues:

If you encounter bugs or have suggestions for improvements, please open an issue on the GitHub repository. Make sure to provide as much detail as possible, including code snippets and error messages, if applicable.

I appreciate your contributions and look forward to improving this library together!

