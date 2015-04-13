/*
 * Copyright (C) 2015 Bitbot
 */

#pragma once

#include "helix/core/order_book.hh"

#include <cstddef>
#include <vector>
#include <string>

namespace helix {

namespace core {

class order_book;

enum class trade_sign {
    buyer_initiated,
    seller_initiated,
    crossing,
    exchange_initiated,
};

struct trade {
    std::string symbol;
    uint64_t    timestamp;
    uint64_t    price;
    trade_sign  sign;

    trade(const std::string& symbol_, uint64_t timestamp_, uint64_t price_, trade_sign sign_)
        : symbol{symbol_}
        , timestamp{timestamp_}
        , price{price_}
        , sign{sign_}
    { }
};

using ob_callback = std::function<void(const order_book&)>;

using trade_callback = std::function<void(const trade&)>;

// FIXME: consolidate these

struct venue {
};

struct symbol {
};

struct subscription {
    virtual void unsubsribe();
};

struct connection {
    virtual subscription* subscribe();
};

struct error {
};

class session {
public:
    virtual void process_packet(const char *buf, size_t size) = 0;
};

class protocol {
public:
    virtual session* new_session(const std::vector<std::string>&, ob_callback, trade_callback) = 0;
};

}

}
