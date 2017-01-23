#ifndef PROTO11_CPP11_EMITTER_HPP_INCLUDED
#define PROTO11_CPP11_EMITTER_HPP_INCLUDED

#include <sstream>
#include <string>
#include <utility>

namespace proto11 {

constexpr int INDENT_SIZE = 4;

class Emitter {
    enum class Mode {
        Headers,
        Body,
        Tail,
        Cpp,
    };

    Mode _mode = Mode::Body;

    int _headers_indent = 0;
    int _body_indent = 0;
    int _tail_indent = 0;
    int _cpp_indent = 0;

    std::stringstream _headers_strm;
    std::stringstream _body_strm;
    std::stringstream _tail_strm;
    std::stringstream _cpp_stream;

    int& _current_indent() {
        switch (_mode) {
        case Mode::Headers:
            return _headers_indent;
        case Mode::Body:
            return _body_indent;
        case Mode::Tail:
            return _tail_indent;
        case Mode::Cpp:
            return _cpp_indent;
        }
        std::terminate();
    }

    std::stringstream& _current_stream() {
        switch (_mode) {
        case Mode::Headers:
            return _headers_strm;
        case Mode::Body:
            return _body_strm;
        case Mode::Tail:
            return _tail_strm;
        case Mode::Cpp:
            return _cpp_stream;
        }
        std::terminate();
    }

public:
    Emitter() = default;

    void emit() {}

    template <typename T, typename... Args> void emit(T&& t, Args&&... args) {
        _current_stream() << std::forward<T>(t);
        emit(std::forward<Args>(args)...);
    }

    template <typename... Args> void line(Args&&... args) {
        emit(std::string(_current_indent(), ' '), std::forward<Args>(args)..., '\n');
    }

    template <typename Fn> void indented(Fn fn) {
        _current_indent() += INDENT_SIZE;
        try {
            fn();
            _current_indent() -= INDENT_SIZE;
        } catch (...) {
            _current_indent() -= INDENT_SIZE;
            throw;
        }
    }

    template <typename Head, typename Tail, typename Fn> void block(Head&& hd, Tail&& tl, Fn fn) {
        line(hd);
        indented(fn);
        line(tl);
    }

    std::string get_string() const {
        return _headers_strm.str() + "\n\n" + _body_strm.str() + "\n\n" + _tail_strm.str();
    }

    std::string get_cpp_string() const {
        return _cpp_stream.str();
    }

    template <typename Fn> void headers(Fn fn) {
        const auto old_mode = _mode;
        try {
            _mode = Mode::Headers;
            fn();
            _mode = old_mode;
        } catch (...) {
            _mode = old_mode;
            throw;
        }
    }

    template <typename Fn> void tail(Fn fn) {
        const auto old_mode = _mode;
        try {
            _mode = Mode::Tail;
            fn();
            _mode = old_mode;
        } catch (...) {
            _mode = old_mode;
            throw;
        }
    }
};
}

#endif  // PROTO11_CPP11_EMITTER_HPP_INCLUDED