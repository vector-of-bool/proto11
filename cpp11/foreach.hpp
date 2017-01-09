#ifndef PROTO11_CPP11_FOREACH_HPP_INCLUDED
#define PROTO11_CPP11_FOREACH_HPP_INCLUDED

#include <functional>

namespace proto11 {

template <typename Container, typename Contained> struct foreach_helper {
    std::reference_wrapper<const Container> _container;
    using counter_fn = int (Container::*)() const;
    using getter_fn = Contained* (Container::*)(int) const;
    counter_fn _counter;
    getter_fn _getter;

    foreach_helper(const Container& c, counter_fn counter, getter_fn getter)
        : _container(c)
        , _counter(counter)
        , _getter(getter) {}
    // template <typename Container>
    template <typename Fn> void operator->*(Fn fn) {
        auto& cont = _container.get();
        const auto count = (cont.*_counter)();
        for (auto i = 0; i < count; ++i) {
            auto& elem = *(cont.*_getter)(i);
            fn(elem);
        }
    }
};

template <typename Container, typename Contained>
foreach_helper<Container, Contained> make_foreach_helper(const Container& c,
                         int (Container::*counter)() const,
                         Contained* (Container::*getter)(int) const) {
    return foreach_helper<Container, Contained>(c, counter, getter);
}
}

#define PROTO11_FOREACH(container, attribute)                                                                          \
    ::proto11::make_foreach_helper(container,                                                                          \
                                   &std::decay<decltype(container)>::type::attribute##_count,                          \
                                   &std::decay<decltype(container)>::type::attribute)                                  \
        ->* [&](decltype(*container.attribute(0))& attribute) -> void

#endif  // PROTO11_CPP11_FOREACH_HPP_INCLUDED