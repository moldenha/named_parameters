// This is a struct to hold values in order to allow perfect fowarding
// It can hold any type of value

#ifndef NT_NAMED_PARAMETERS_VALUE_HOLDER_HPP__
#define NT_NAMED_PARAMETERS_VALUE_HOLDER_HPP__


namespace nt::named_arguments{

template <typename T>
struct value_holder {
    T value;

    template <typename U>
    constexpr explicit value_holder(U&& u) : value(std::forward<U>(u)) {}

    constexpr T&& get() && { return std::move(value); }
    constexpr T& get() & { return value; }
    constexpr const T& get() const& { return value; }
};

// r-value reference partial specialization
// To avoid slicing rvalues or binding to temporaries incorrectly
template <typename T>
struct value_holder<T&> {
    T* ptr;

    constexpr explicit value_holder(T& t) : ptr(&t) {}

    constexpr T& get() const { return *ptr; }
};

template <typename T>
struct value_holder<const T&> {
    const T* ptr;

    constexpr explicit value_holder(const T& t) : ptr(&t) {}

    constexpr const T& get() const { return *ptr; }
};


} //nt::named_parameters::

#endif
