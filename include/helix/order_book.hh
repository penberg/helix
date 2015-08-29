#pragma once

/// \defgroup order-book Order book management
///
/// Order book management provides support for reconstructing and
/// querying per-asset order book state such as top and depth of book bid
/// and ask price and size.

#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>

#include <unordered_map>
#include <cstdint>
#include <utility>
#include <memory>
#include <string>
#include <list>
#include <map>

namespace helix {

namespace core {

/// \addtogroup order-book
/// @{

/// \brief Order side.
enum class side_type {
    /// Buy order
    buy,
    /// Sell order
    sell,
};

/// \brief Trading state.
enum class trading_state {
    /// Trading state is unknown.
    unknown,
    /// Trading is halted.
    halted,
    /// Trading is paused.
    paused,
    /// Quotation only period.
    quotation_only,
    /// Trading is ongoing.
    trading,
    /// Auction period.
    auction,
};

struct price_level;

/// \brief Order is a request to buy or sell quantity of asset at a
/// specified price.
struct order final {
    price_level* level;
    uint64_t     id;
    uint64_t     price;
    uint64_t     quantity;
    side_type    side;
    uint64_t     timestamp;

    order(uint64_t id, uint64_t price, uint64_t quantity, side_type side, uint64_t timestamp)
        : level{nullptr}
        , id{id}
        , price{price}
        , quantity{quantity}
        , side{side}
        , timestamp{timestamp}
    {}

    order(const order&) = default;
    order(order&&) = default;
    order& operator=(const order&) = default;
};

/// \brief Price level is a time-prioritized list of orders with the same price.
struct price_level {
    price_level(uint64_t price_)
        : price(price_)
        , size(0)
    { }

    price_level(const price_level&) = default;
    price_level& operator=(const price_level&) = default;

    uint64_t price;
    uint64_t size;
};

/// \brief Order book is a price-time prioritized list of buy and sell
/// orders.

using namespace boost::multi_index;

typedef multi_index_container<order, indexed_by<
    hashed_unique<member<order, uint64_t, &order::id>>>> order_set;

class order_book {
private:
    std::string _symbol;
    uint64_t _timestamp;
    trading_state _state;
    order_set _orders;
    std::map<uint64_t, price_level, std::greater<uint64_t>> _bids;
    std::map<uint64_t, price_level, std::less   <uint64_t>> _asks;
public:
    using iterator = order_set::iterator;

    order_book(const std::string& symbol, uint64_t timestamp, size_t max_orders = 0);
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
    void replace(uint64_t order_id, order&& order);
    void cancel(uint64_t order_id, uint64_t quantity);
    std::pair<uint64_t, side_type> execute(uint64_t order_id, uint64_t quantity);
    void remove(uint64_t order_id);
    side_type side(uint64_t order_id) const;

    size_t bid_levels() const;
    size_t ask_levels() const;
    size_t order_count() const;

    uint64_t bid_price(size_t level) const;
    uint64_t bid_size (size_t level) const;
    uint64_t ask_price(size_t level) const;
    uint64_t ask_size (size_t level) const;
    uint64_t midprice (size_t level) const;

private:
    void remove(iterator& iter);

    template<typename T>
    void remove(const order& o, T& levels);

    template<typename T>
    price_level& lookup_or_create(T& levels, uint64_t price);
};

/// @}

}

}