import sys

def print_reflect_constructor_macro(num):
    print("#define _NT_REFLECT_CONSTRUCTOR_SELECT_MACRO_(", end= '')
    for i in range(num):
        print("_{}, ".format(i+1), end = '')
    print("NAME, ...) NAME")



def print_make_combination_out_macro(num):
    print("#define _NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_{}_(".format(num), end = '')
    if num == 0:
        print(")")
        return
    for i in range(num):
        print("arg{}_a, ".format(i+1), end = '')
    for i in range(num-1):
        print("arg{}_b, ".format(i+1), end = '')
    print("arg{}_b) ".format(num), end = '')
    for i in range(num-1):
        print("arg{}_a arg{}_b, ".format(i+1, i+1), end='')
    print("arg{}_a arg{}_b".format(num, num))

    
def print_whole_make_combination_out_macro(num):
    for i in range(num+1):
       print_make_combination_out_macro(i)
    print("#define _NT_NTNAMED_ARGS_EXPAND_TYPES_FOR_CLASS_MACRO_(types_holder, names_holder) _NT_REFLECT_CONSTRUCTOR_SELECT_MACRO_(types_holder, ", end = '')
    for i in reversed(range(num+1)):
        if i == 0: break
        print("_NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_{}_, ".format(i), end = '')
    print("_NT_COMBINE_NAMED_PARAMETER_AND_TYPE_MACRO_0_)(types_holder, names_holder) ")



def print_make_ntarg_generate_macro(num):
    print("#define _NT_MAKE_NTARG_MACRO_{}_(".format(num), end = '')
    if num == 0:
        print(")")
        return
    for i in range(num-1):
        print("arg{}, ".format(i+1), end = '')
    print("arg{}) ".format(num), end = '')
    for i in range(num-1):
        print("ntarg_(arg{}), ".format(i+1), end = '')
    print("ntarg_(arg{})".format(num))

def print_whole_make_ntarg_generate_macro(num):
    for i in range(num+1):
        print_make_ntarg_generate_macro(i)
    print("#define _NT_MAKE_NTARGS_MACRO_EXPAND_(...) _NT_REFLECT_CONSTRUCTOR_SELECT_MACRO_(__VA_ARGS__, ", end = '')
    for i in reversed(range(num+1)):
        if i == 0: break
        print("_NT_MAKE_NTARG_MACRO_{}_, ".format(i), end = '')
    print("_NT_MAKE_NTARG_MACRO_0_)(__VA_ARGS__) ")


def make_whole_file(filename, num, num_b = None):
    if(num_b == None):
        num_b = num
    file = open(filename, 'w')
    original_stdout = sys.stdout
    sys.stdout = file
    print("#ifndef __NT_NAMED_ARGUMENT_PARAMETERS_EXPANDED_CLASS_CONSTRUCTOR_MACROS_H__") #starting the file
    print("#define __NT_NAMED_ARGUMENT_PARAMETERS_EXPANDED_CLASS_CONSTRUCTOR_MACROS_H__")
    print()
    print()
    print()
    print_reflect_constructor_macro(num)
    print_whole_make_ntarg_generate_macro(num)
    print_whole_make_combination_out_macro(num_b)
    print()
    print()
    print()
    print("#endif") #ending the file
    sys.stdout = original_stdout
    file.close()


if __name__ == '__main__':
    make_whole_file("expanded_class_constructor_macros.h", 100, 62) #62 keeps it below MSVC limit


