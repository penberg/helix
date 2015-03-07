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

using callback = std::function<void(const order_book&)>;

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

struct trade {
};

class session {
public:
    virtual void process_packet(const char *buf, size_t size) = 0;
};

class protocol {
public:
    virtual session* new_session(const std::vector<std::string>&, callback) = 0;
};

}

}
