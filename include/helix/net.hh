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
};

}

}
