#include "helix/order_book.hh"

#include <boost/version.hpp>
#include <stdexcept>
#include <limits>

namespace helix {

execution::execution(uint64_t price, side_type side, uint64_t remaining)
    : price{price}
    , side{side}
    , remaining{remaining}
{
}

order_book::order_book(std::string symbol, uint64_t timestamp, size_t max_orders)
    : _symbol{std::move(symbol)}
    , _timestamp{timestamp}
    , _state{trading_state::unknown}
{
#if BOOST_VERSION >= 105600
    _orders.reserve(max_orders);
#endif
}

void order_book::add(order order)
{
    switch (order.side) {
    case side_type::buy: {
        auto&& level = lookup_or_create(_bids, order.price);
        order.level = &level;
        level.size += order.quantity;
        break;
    }
    case side_type::sell: {
        auto&& level = lookup_or_create(_asks, order.price);
        order.level = &level;
        level.size += order.quantity;
        break;
    }
    default:
        throw std::invalid_argument(std::string("invalid side: ") + static_cast<char>(order.side));
    }
    _orders.emplace(std::move(order));
}

void order_book::replace(uint64_t order_id, order order)
{
    remove(order_id);
    add(std::move(order));
}

void order_book::cancel(uint64_t order_id, uint64_t quantity)
{
    auto it = _orders.find(order_id);
    if (it == _orders.end()) {
        throw std::invalid_argument(std::string("invalid order id: ") + std::to_string(order_id));
    }
    _orders.modify(it, [quantity](order& order) {
        order.quantity -= quantity;
        order.level->size -= quantity;
    });
    if (!it->quantity) {
        remove(it);
    }
}

execution order_book::execute(uint64_t order_id, uint64_t quantity)
{
    auto it = _orders.find(order_id);
    if (it == _orders.end()) {
        throw std::invalid_argument(std::string("invalid order id: ") + std::to_string(order_id));
    }
    _orders.modify(it, [quantity](order& order) {
        order.quantity -= quantity;
        order.level->size -= quantity;
    });
    auto result = execution(it->price, it->side, it->level->size);
    if (!it->quantity) {
        remove(it);
    }
    return result;
}

void order_book::remove(uint64_t order_id)
{
    auto it = _orders.find(order_id);
    if (it == _orders.end()) {
        throw std::invalid_argument(std::string("invalid order id: ") + std::to_string(order_id));
    }
    remove(it);
}

void order_book::remove(iterator& iter)
{
    auto && order = *iter;
    switch (order.side) {
    case side_type::buy: {
        remove(order, _bids);
        break;
    }
    case side_type::sell: {
        remove(order, _asks);
        break;
    }
    default:
        throw std::invalid_argument(std::string("invalid side: ") + static_cast<char>(order.side));
    }
    _orders.erase(iter);
}

template<typename T>
void order_book::remove(const order& o, T& levels)
{
    auto it = levels.find(o.price);
    if (it == levels.end()) {
        throw std::invalid_argument(std::string("invalid price: ") + std::to_string(o.price));
    }
    auto&& level = it->second;
    o.level->size -= o.quantity;
    if (level.size == 0) {
        levels.erase(it);
    }
}

template<typename T>
price_level& order_book::lookup_or_create(T& levels, uint64_t price)
{
    price_level level{price};
    auto it = levels.emplace(price, level).first;

    return it->second;
}

side_type order_book::side(uint64_t order_id) const
{
    auto it = _orders.find(order_id);
    if (it == _orders.end()) {
        throw std::invalid_argument(std::string("invalid order id: ") + std::to_string(order_id));
    }
    return it->side;
}


size_t order_book::bid_levels() const
{
    return _bids.size();
}

size_t order_book::ask_levels() const
{
    return _asks.size();
}

size_t order_book::order_count() const
{
    return _orders.size();
}

uint64_t order_book::bid_price(size_t level) const
{
    auto it = _bids.begin();
    while (it != _bids.end() && level--) {
        it++;
    }
    if (it != _bids.end()) {
        auto&& level = it->second;
        return level.price;
    }
    return std::numeric_limits<uint64_t>::min();
}

uint64_t order_book::bid_size(size_t level) const
{
    auto it = _bids.begin();
    while (it != _bids.end() && level--) {
        it++;
    }
    if (it != _bids.end()) {
        auto&& level = it->second;
        return level.size;
    }
    return 0;
}

uint64_t order_book::ask_price(size_t level) const
{
    auto it = _asks.begin();
    while (it != _asks.end() && level--) {
        it++;
    }
    if (it != _asks.end()) {
        auto&& level = it->second;
        return level.price;
    }
    return std::numeric_limits<uint64_t>::max();
}

uint64_t order_book::ask_size(size_t level) const
{
    auto it = _asks.begin();
    while (it != _asks.end() && level--) {
        it++;
    }
    if (it != _asks.end()) {
        auto&& level = it->second;
        return level.size;
    }
    return 0;
}

uint64_t order_book::midprice(size_t level) const
{
    auto bid = bid_price(level);
    auto ask = ask_price(level);
    return (bid + ask) / 2;
}

}
