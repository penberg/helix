#pragma once

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

    template<typename T>
    const T* cast() const {
        return reinterpret_cast<const T*>(_buf);
    }
};

class message_parser {
public:
    virtual ~message_parser()
    { }

    virtual size_t parse(const packet_view&) = 0;
};

}

}
