# NeuroTensor Named Parameters

**v0.1.1**

This is a named parameters library that was developed for [NeuroTensor](https://github.com/moldenha/NeuroTensor)

This library is cross platform C++17 compatible. The design was based around a few different needs for NeuroTensor having named parameters. This library deduces types, named types, and argument placement at compile time. It offers standard function named parameters, the ability to deduce which function should be used for an overload, and named parameters for class constructors. You will notice that most of the syntax has `nt` in the name, if you want to use the library outside of the intended NeuroTensor scope and do not want that syntax, you can go to the `macros.h` file and fairly easily change it.


## User Usage

The goal of this library is to have a fairly seamless use for users, and parameters can be used in any order. All that has to be included is `named_parameters.hpp`. Users may use functions as such:

```cpp
// foo.h
void foo(int a, int b, int c, double d);

// main.cpp
foo(3, d = 3.4, b = 5)

```

The setup to allow this to happen is fairly easy to implement from a developer standpoint.

```cpp
// foo.h

// It doesn't have to be in a namespace, it can be a different named function
namespace function_implementation{
void foo(int a, int b, int c, double d){
    std::cout << "adding them all up is "<<
       static_cast<double>(a+b+c)+d<<std::endl;
}

}

NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo) //output function name for user to use
    ntarg_(a), //required argument
    ntarg_(b), //required argument
    ntarg_(c) = 3, //default argument
    ntarg_(d) = 3.4 //default argument
NT_FINISH_NAMED_PARAMETER_FUNCTION_(function_implementation::foo) //the implementation of the function

// main.cpp

// User usage:
foo(3, ntarg_(d) = 3.4, ntarg_(b) = 5)
```

### Extending User Ease

In the initial example, it was shown that foo could be used by a user as `foo(3, d = 3.4, b = 5)` instad of `foo(3, ntarg_(d) = 3.4, ntarg_(b) = 5)`. 
From a developer standpoint, all that needs to be done is to pre-define the name as such: `inline static constexpr auto name = ntarg_(name)`. This is how it is done in the above example:

```cpp
// foo.h

// It doesn't have to be in a namespace, it can be a different named function
namespace function_implementation{
void foo(int a, int b, int c, double d){
    std::cout << "adding them all up is "<<
       static_cast<double>(a+b+c)+d<<std::endl;
}

}

NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo) //output function name for user to use
    ntarg_(a), //required argument
    ntarg_(b), //required argument
    ntarg_(c) = 3, //default argument
    ntarg_(d) = 3.4 //default argument
NT_FINISH_NAMED_PARAMETER_FUNCTION_(function_implementation::foo) //the implementation of the function

// This is all that is changeed in foo.h:
inline static constexpr auto a = ntarg_(a);
inline static constexpr auto b = ntarg_(b);
inline static constexpr auto c = ntarg_(c);
inline static constexpr auto d = ntarg_(d);

// main.cpp

// User usage:
foo(3, d = 3.4, b = 5)
```



This named parameters library also allows for function overloads using `NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_` instead of `NT_FINISH_NAMED_PARAMETER_FUNCTION_`. The correct function overload and return types will be automatically deduced for you just like when there were no overloads on the functions. If any templates or special conditions such as the use of `std::enable_if_t` are used, that will also be deduced for you.
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
NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = 20,
    ntarg_(d) = 20.32
NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(overloaded_functions::foo)




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
    
    NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_(MaxPool2D,
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(kernel_size, stride, padding, dilation, return_indices, ceil_mode),
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_(-1, 0, 1, false, false), // everything but kernel_size has a default argument
        NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_(utils::my_n_tuple<2>, utils::my_n_tuple<2>, utils::my_n_tuple<2>, utils::my_n_tuple<2>, bool, bool) )
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

The use of `ntarg_` is pretty simple, it takes the variable name `ntarg_(a)` is the parameter name `a` and then it can be set to the value using `ntarg_(a) = 10`. Names can also be pre-defined so that a user does not have to use `ntarg_` when calling a name, only the actual name. This can be done by defining `inline static constexpr auto name = ntarg_(name)`

### Overloading Functions `NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_`

When dealing with functions that have only one definition and are not overloaded, you can end the beggining construction that starts with `NT_MAKE_NAMED_PARAMETER_FUNCTION_` that takes the argument of the outfunction name, with `NT_FINISH_NAMED_PARAMETER_FUNCTION_` of where the function was originally defined.

Howver, when dealing with overloaded functions, you must end the definition with `NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_`, followed by the name of the original function. This is just like using `NT_MAKE_NAMED_PARAMETER_FUNCTION_` but with a different name.


```cpp
//example:
NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(overloaded_namespace::foo)

```

#### Determining the correct overload

The correct overload is automatically determined for you, and it is determined the same way the standard does. So if calling the function without using named parameters would cause an ambiguity error, so would calling it with named parameters. The use of templates or specific conditions such as the use of `std::enable_if_t` can be used upon the function with no issue.


### Class Construction `NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_`

This is a way to define a class constructor that can take named parameter arguments. 
It defines two different constructors:
```cpp
template<typename... Args>
ClassName(Args&&...)

//and:
ClassName(/* all specified types and names */)
```


#### Using `NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_`

`NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_` takes four arguments, the first is the name of the class. The second is a `NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_` which should be filled with the names of the arguments. The third is a `NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_` which should be filled with any pertaining default values. The last is the `NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_TYPES_` which should have the types of each named arguments.

Using `NT_NAMED_CLASS_CONSTRUCTOR_CLASS_DEFAULT_VALS_` and `NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_` happens in reverse. For example: `NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(a, b, c)` and `NT_NAMED_CLASS_CONSTRUCTOR_CLASS_ARG_NAMES_(20, 20.3)` will make `c` have the default argument `20.3` and `b` have the default argument `20` but `a` does not have a default argument.

#### Limitations

Using the macro `NT_MAKE_NAMED_ARGUMENT_CLASS_CONSTRUCTOR_` does not have native support for template arguments in the class definition. Instead, you would have to do something like this (see the use of `nt_auto_` and `nt_convertible_` below):

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
                ), NT_OVERLOAD_PARAMETERS_(void, nt_auto_, int, int){},
                std::forward<Args>(args)...))
            ) {;}
        
        template<typename T>
        MyClass(T a, int b, int c)
        :a(static_cast<int>(a)), b(b), c(c)
        {}
};

```

### Argument Templates `nt_auto_`

Templates are a vital part of C++, and not being able to have functions with template parameters would add some serious limitations to the library. When it comes to non-overloaded functions, they are just handled automatically and you do not have to worry about it. HoIver, when it comes to overloaded functions, and you have to explicitly write out each type, there is a way to handle templates built into the library using `nt_auto_`.

#### Using `nt_auto_` 

Take the class below:
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
                ), NT_OVERLOAD_PARAMETERS_(void, nt_auto_&, int, int){},
                std::forward<Args>(args)...))
            ) {;}
        
        template<typename T>
        MyClass(T& a, int b, int c)
        :a(static_cast<int>(a)), b(b), c(c)
        {}
};
```


**IMPORTANT:** Notice how in the constructor, `T&` is a reference, for that reason in `NT_OVERLOAD_PARAMETERS_(int, nt_auto_&, int, double)` the reference `nt_auto_&` was used. The same follows for `const` or `volatile` expressions. It should be noted that `nt_auto_` will basically just convert to anything as long as the aforementioned implications are kept in mind so `const SomeClass<T>&` as an argument would still be `const nt_auto_&`

`nt_auto_` can also be used for class constructors, just make sure to put `nt_auto_` as one of the types.

### Convertible Types `nt_convertible_`

Some types are implictly meant to be converted into or constructed into. For example `nt::Scalar` is meant to take any scalar element and convert it to the correct type based on the `dtype` of any `nt::Tensor`. For that reason, when overloading a function, one of the noted types should be `nt_convertible_<function_type>`. The library will check to see if it is valid by seeing if it is `std::is_convertible_v`.

### Initializer Liss

C++ Does not allow a function defined like such `template<typename T> foo(T)` to be called with an initializer list like such `foo({1, 2, 3})`. This inheritly goes against the way that the named parameters library works if you want an initializer list to be a parameter for a class such as an `std::vector`.

```C++

//The following is a function that can expect an initializer list:
int foo_(int a, std::vector<int> b, bool c, std::vector<int> d, std::vector<int> e, int f){return 0;}
int foo_(float a, std::vector<int> b, bool c, std::vector<int> d, std::vector<int> e, int f){std::cout << "vector d is "<<d<<std::endl; std::cout << "and b is "<<b<<std::endl;return 0;}


//And normally, this might be how a named parameter foo might be created:

NT_MAKE_NAMED_PARAMETER_FUNCTION_(foo)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = false,
    ntarg_(d) = {3, 4},
    ntarg_(e) = {6, 7},
    ntarg_(f) = 25
NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_(foo_,)

```

With the above, if a user tried to do something like `foo(10, {1, 2, 3})` it would cause an error. So instead, if an initializer list should be an allowed argument, it has to be explicitly specified using `NT_MAKE_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_` and `NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_` or `NT_FINISH_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_` where `NT_MAKE_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_` is called the same way, but `NT_FINISH_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_` and `NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_` expect the function name and number of arguments as parameters. This is an example of how the above should be done properly:

```C++
NT_MAKE_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(foo)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = false,
    ntarg_(d) = {3, 4},
    ntarg_(e) = {6, 7},
    ntarg_(f) = 25
NT_OVERLOAD_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(foo, 6, foo_)
//notice it is name of function user can use, number of parameters, then everything is the same

```

Or if you are not overloading, the following can be done:

``` C++

//The following is a function that can expect an initializer list:
int foo_(int a, std::vector<int> b, bool c, std::vector<int> d, std::vector<int> e, int f){return 0;}


//And normally, this might be how a named parameter foo might be created:

NT_MAKE_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(foo)
    ntarg_(a),
    ntarg_(b),
    ntarg_(c) = false,
    ntarg_(d) = {3, 4},
    ntarg_(e) = {6, 7},
    ntarg_(f) = 25
NT_FINISH_NAMED_PARAMETER_FUNCTION_WITH_INIT_LIST_(foo, 6, foo_)

```

Now initializer lists can be reliably used as parameters.

### Cross Compatibility

This library does not rely on the trick of using `template <typename CharT, CharT... Chars> constexpr NT_StringLiteral_<Chars...> operator"" ntarg_()`. This is a `gcc` only extension for C++. This library aims to be cross compatible against all operating systems, while relying only on the standard C++ library. It is still able to deduce the name of each argument at compile time using `ntarg_`. The name is deduced in a constexpr fashion. `ntarg_` is a macro that just simplifies the verbosity of implementing a named argument. There are no limits to how large a parameter name can be, and a parameter name must be at least one character. Placement of arguments, function overloading, class constructing, and everything that goes into that is evaluated at compile time.

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
    
    *   **Overload Resolution**: Currently the library relies on different macros for when there are overloads, I would like to change that.
        
    *   **Class Constructors**: The class constructor feature works well for most cases, but if you encounter limitations, like handling more complex types or better support for template arguments, I encourage you to improve it. 
        
    *   **Template Arguments Support**: I would like to expand the support for template arguments in class constructors.
        
3.  **Improve Cross-Platform Compatibility**:
    
    *   While this library is designed to be cross-platform and C++17 compatible, there may be edge cases where certain environments or compilers behave differently. If you find any compatibility issues, please contribute a fix or improvement.
        
4.  **Performance Improvements**:
    
    *   As the library uses several macros and template metaprogramming techniques, there might be opportunities for performance optimizations, especially for large-scale use cases. If you have any performance optimization suggestions, feel free to share! I am currently going through and reducing the number of macros that are in the library. There have already been considerable improvements from version 0.1.0 to 0.1.1, I plan to add many more.
        

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

