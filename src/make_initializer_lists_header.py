import sys

start = """// C++ by default does not allow a function with a template argument to take an initializer list
// Therefore, in order to address that, there needs to be a wrapper for the function
// This can be made below with macros
#include <vector>
#include "utils_macros.h"

#ifndef NT_REFLECTION_NAMED_ARGUMENTS_INITIALIZER_LIST_HPP__
#define NT_REFLECTION_NAMED_ARGUMENTS_INITIALIZER_LIST_HPP__

namespace nt::named_arguments::details{
template<typename T>
struct is_initializer_list : std::false_type {};

template<typename T>
struct is_initializer_list<std::initializer_list<T>> : std::true_type {};
}

#define NT_INITIALIZER_LIST_FORWARD(num) std::forward<std::conditional_t<::nt::named_arguments::details::is_initializer_list<std::decay_t<decltype(_##num)>>::value, std::initializer_list<T##num>, T##num> >(_##num)
#define NT_LARG(num) std::initializer_list<T##num>&& _##num
#define NT_TARG(num) T##num&& _##num
"""


end = '#endif'

def print_combo_num(num):
    if num == 1:
        print("#define NT_NAMED_INITIALIZER_COMBO1(MCNAME, N, f1, f2, ...) MCNAME(N, f1, f2, (__VA_ARGS__))")
    elif num == 2:
        string = '''#define NT_NAMED_INITIALIZER_COMBO2(MCNAME, N, f1, f2, ...) MCNAME(N, f1, f2, (NT_LARG(1),__VA_ARGS__))\\
                                                            MCNAME(N, f1, f2, (NT_TARG(1), __VA_ARGS__))'''
        print(string)
    else:
        string = """#define NT_NAMED_INITIALIZER_COMBO{}(MCNAME, N, f1, f2, ...) NT_NAMED_INITIALIZER_COMBO{}(MCNAME, N, f1, f2, NT_LARG({}),__VA_ARGS__)\\
                                                            NT_NAMED_INITIALIZER_COMBO{}(MCNAME, N, f1, f2, NT_TARG({}), __VA_ARGS__)""".format(num, num-1, num-1, num-1, num-1)
        print(string)

def print_combos(nums):
    for i in reversed(range(nums+1)):
        if i == 0: break
        print_combo_num(i)


def print_Tcombo_num(num):
    if num == 1:
        print("#define NT_NAMED_INITIALIZER_TCOMBO1 typename T1")
    else:
        print("#define NT_NAMED_INITIALIZER_TCOMBO{} NT_NAMED_INITIALIZER_TCOMBO{}, typename T{}".format(num, num-1, num))

def print_Tcombo(nums):
    for i in range(nums+1):
        if i == 0: continue
        print_Tcombo_num(i)

def print_ILcombo_num(num):
    if num == 1:
        print("#define NT_NAMED_INITIALIZER_NCOMBO1 NT_INITIALIZER_LIST_FORWARD(1)")
    else:
        print("#define NT_NAMED_INITIALIZER_NCOMBO{} NT_NAMED_INITIALIZER_NCOMBO{}, NT_INITIALIZER_LIST_FORWARD({})".format(num, num-1, num))

def print_ILcombo(nums):
    for i in range(nums+1):
        if i == 0: continue
        print_ILcombo_num(i)

def print_generalN_macros():
    string = """
#define NT_MAKE_INITIALIZER_LIST_FUNCTION(func_name, func_location, TEMPLATES, ARGS, NAMES)\\
template<TEMPLATES>\\
inline decltype(auto) func_name(ARGS){return func_location(NAMES);}

#define NT_NAMED_IL_FUNCN(N, f1, f2, ARGS)\\
    NT_MAKE_INITIALIZER_LIST_FUNCTION(f1, f2, NT_NAMED_INITIALIZER_TCOMBO##N, NT_EXPAND ARGS, NT_EXPAND( NT_NAMED_INITIALIZER_NCOMBO##N ))



#define NT_MAKE_NAMED_IL_FUNCN(N, f1, f2)\\
    NT_NAMED_ARGUMENTS_CONCAT_(NT_NAMED_INITIALIZER_COMBO,N)(NT_NAMED_IL_FUNCN, N, f1, f2, NT_TARG(N))\\
    NT_NAMED_ARGUMENTS_CONCAT_(NT_NAMED_INITIALIZER_COMBO,N)(NT_NAMED_IL_FUNCN, N, f1, f2, NT_LARG(N))
        """
    print(string)

def print_reflect_func_num(num):
    if num == 1:
        print("#define NT_REFLECT_NAMED_INITIALIZER_LIST_FUNCTION_1(func_name, func_location) NT_MAKE_NAMED_IL_FUNCN(1, func_name, func_location)")
    else:
        print("#define NT_REFLECT_NAMED_INITIALIZER_LIST_FUNCTION_{}(func_name, func_location) NT_MAKE_NAMED_IL_FUNCN({}, func_name, func_location) NT_REFLECT_NAMED_INITIALIZER_LIST_FUNCTION_{}(func_name, func_location) ".format(num, num, num-1))

def print_reflect_func(nums):
    for i in range(nums+1):
        if i == 0: continue
        print_reflect_func_num(i)
 
def make_whole_file(filename, num):
    file = open(filename, 'w')
    original_stdout = sys.stdout
    sys.stdout = file
    print(start)
    print()
    print_combos(num)
    print()
    print_Tcombo(num)
    print()
    print_ILcombo(num)
    print()
    print_generalN_macros()
    print()
    print_reflect_func(num)
    print()
    print("#endif") #ending the file
    sys.stdout = original_stdout
    file.close()

if __name__ == '__main__':
    make_whole_file("initializer_list.hpp", 100)



