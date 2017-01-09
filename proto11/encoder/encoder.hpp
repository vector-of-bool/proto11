#ifndef PROTO11_ENCODER_ENCODER_HPP_INCLUDED
#define PROTO11_ENCODER_ENCODER_HPP_INCLUDED

#include <utility>

#include <boost/optional.hpp>

namespace proto11 {

namespace detail {

template <typename T> struct void_t_helper { using type = void; };
}

template <typename T> using void_t = typename detail::void_t_helper<T>::type;

template <typename T, typename = void> struct traits;

template <typename Type, typename Iter, typename = typename traits<Type>::check>
Iter write(const Type& what, Iter out) {
    return traits<Type>::write(what, out);
}

template <typename Type, typename Iter, typename CheckValid = typename traits<Type>::check>
std::pair<Type, Iter> read(Iter first, Iter last) {
    return traits<Type>::read(first, last);
}

template <> struct traits<std::string, void> {
    enum {
        valid = true,
        is_enum = false,
        is_message = false,
    };
    using check = std::true_type;
    using type = std::string;
    template <typename Iter> static Iter write(const type& what, Iter iter) {
        return std::copy(what.begin(), what.end(), iter);
    }
    template <typename Iter> static std::pair<type, Iter> read(Iter first, Iter last) {
        return { std::string(first, last), last };
    }
};

template <> struct traits<std::int32_t, void> {
    enum {
        valid = true,
        is_enum = false,
        is_message = false,
    };
    using check = std::true_type;
    using type = std::int32_t;
    template <typename Iter> static Iter write(const type what, Iter iter) { return write_varint(what, iter); }
    template <typename Iter> static std::pair<type, Iter> read(Iter first, Iter last) {
        return read_varint(first, last);
    }
};

template <typename Type> struct traits<boost::optional<Type>, void_t<typename traits<Type>::check>> {
    enum {
        valid = true,
        is_enum = false,
        is_message = false,
    };
    using check = std::true_type;
    using type = boost::optional<Type>;
    template <typename Iter> static Iter write(const type& what, Iter iter) {
        if (what) {
            iter = proto11::write(*what, iter);
        }
        return iter;
    }
    template <typename Iter> static std::pair<type, Iter> read(Iter iter, const Iter last) {
        return proto11::read<Type>(iter, last);
    }
};

}  // namespace proto11

#endif  // PROTO11_ENCODER_ENCODER_HPP_INCLUDED