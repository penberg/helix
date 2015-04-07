/*
 * Copyright (C) 2015 Bitbot
 */

#pragma once

#include <unordered_map>
#include <cstdint>
#include <memory>
#include <list>
#include <map>

namespace helix {

namespace core {

enum class side {
    buy,
    sell,
};

enum class trading_state {
    unknown,
    halted,
    trading,
    auction,
};

struct price_level;

struct order {
    order(uint64_t id_, uint64_t price_, uint64_t quantity_, side side_)
        : level(nullptr)
        , id(id_)
        , price(price_)
        , quantity(quantity_)
        , _side(side_)
    {}

    order(const order&) = default;
    order(order&&) = default;
    order& operator=(const order&) = default;

    price_level* level;
    uint64_t     id;
    uint64_t     price;
    uint64_t     quantity;
    side         _side;
};

// A price level is a time-prioritized list of orders with the same price.
struct price_level {
    price_level(uint64_t price_)
        : price(price_)
        , size(0)
    { }

    price_level(const price_level&) = default;
    price_level& operator=(const price_level&) = default;

    uint64_t price;
    uint64_t size;

    // Order IDs sorted by timestamp (ascending order):
    std::list<uint64_t> orders;
};

class order_book {
private:
    std::string _symbol;
    uint64_t _timestamp;
    trading_state _state;
    std::unordered_map<uint64_t, order> _orders;
    std::map<uint64_t, price_level, std::greater<uint64_t>> _bids;
    std::map<uint64_t, price_level, std::less   <uint64_t>> _asks;
public:
    using iterator = std::unordered_map<uint64_t, order>::iterator;

    order_book(const std::string& symbol, uint64_t timestamp);
    ~order_book();

    order_book(const order_book&) = default;
    order_book(order_book&&) = default;
    order_book& operator=(const order_book&) = default;

    const std::string& symbol() const {
        return _symbol;
    }

    void set_timestamp(uint64_t timestamp) {
        _timestamp = timestamp;
    }

    uint64_t timestamp() const {
        return _timestamp;
    }

    void set_state(trading_state state) {
        _state = state;
    }

    trading_state state() const {
        return _state;
    }

    void add(order&& order);
    void cancel(uint64_t order_id, uint64_t quantity);
    uint64_t execute(uint64_t order_id, uint64_t quantity);
    void remove(uint64_t order_id);

    uint64_t bid_price(size_t level) const;
    uint64_t bid_size (size_t level) const;
    uint64_t ask_price(size_t level) const;
    uint64_t ask_size (size_t level) const;

private:
    void remove(iterator& iter);

    template<typename T>
    void remove(order& o, T& levels);

    template<typename T>
    price_level& lookup_or_create(T& levels, uint64_t price);
};

}

}
