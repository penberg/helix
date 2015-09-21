#pragma once

/// \mainpage
///
/// Helix is an ultra-low latency C++ feed handler.
///
/// The documentation is organized into following sections:
///
///   - \ref order-book Order book reconstruction and management.

#include "helix/order_book.hh"

#include <cstddef>
#include <vector>
#include <string>

namespace helix {

namespace net {

class packet_view;

}

namespace core {

enum class trade_sign {
    buyer_initiated,
    seller_initiated,
    crossing,
    non_displayable,
};

struct trade {
    std::string symbol;
    uint64_t    timestamp;
    uint64_t    price;
    uint64_t    size;
    trade_sign  sign;

    trade(const std::string& symbol_, uint64_t timestamp_,
          uint64_t price_, uint64_t size_, trade_sign sign_)
        : symbol{symbol_}
        , timestamp{timestamp_}
        , price{price_}
        , size{size_}
        , sign{sign_}
    { }
};

using ob_callback = std::function<void(const order_book&)>;

using trade_callback = std::function<void(const trade&)>;

class session {
    void* _data;
public:
    session(void* data)
        : _data{data}
    { }

    virtual ~session()
    { }

    void* data() {
        return _data;
    }

    virtual void subscribe(const std::string& symbol, size_t max_orders) = 0;

    virtual void register_callback(core::ob_callback process_ob) = 0;

    virtual void register_callback(core::trade_callback process_trade) = 0;

    virtual size_t process_packet(const net::packet_view& packet) = 0;
};

class protocol {
public:
    virtual session* new_session(void*) = 0;
};

}

}
