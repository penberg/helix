#pragma once

#include <experimental/string_view>
#include <cstddef>

namespace helix {

namespace net {

class packet_view {
    const char* _buf;
    size_t _len;
public:
    packet_view(const char* buf, size_t len)
        : _buf{buf}
        , _len{len}
    { }

    const char* buf() const {
        return _buf;
    }

    size_t len() const {
        return _len;
    }

    const char* end() const {
        return _buf + _len;
    }

    std::experimental::string_view as_string_view() const {
        return std::experimental::string_view{_buf, _len};
    }

    template<typename T>
    const T* cast() const {
        return reinterpret_cast<const T*>(_buf);
    }
};

}

}
