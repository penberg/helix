#pragma once

#include "helix/parity/pmd_messages.h"
#include "helix/order_book.hh"
#include "helix/helix.hh"
#include "helix/net.hh"

#include <unordered_map>
#include <vector>
#include <memory>
#include <set>

namespace helix {

namespace parity {

// Parity PMD
//
// This is a feed handler for PMD. The handler assumes that transport protocol
// framing such as SoupTCP or MoldUDP has already been parsed and works
// directly on PMD messages.
//
// PMD is specified in:
//
//   https://github.com/jvirtanen/parity/blob/master/parity-net/doc/PMD.md
//
class pmd_handler {
    //! Callback function for processing events.
    core::event_callback _process_event;
    //! A map of order books by order book ID.
    std::unordered_map<std::string, helix::core::order_book> _order_book_id_map;
    //! A map of order books by order ID.
    std::unordered_map<uint64_t, helix::core::order_book&> _order_id_map;
    //! A set of symbols that we are interested in.
    std::set<std::string> _symbols;
    //! Number of seconds since midnight when the trading session started.
    uint32_t _seconds;
public:
    pmd_handler();
    bool is_rth_timestamp(uint64_t timestamp) const;
    void subscribe(std::string sym, size_t max_orders);
    void register_callback(core::event_callback callback);
    size_t process_packet(const net::packet_view& packet);
private:
    template<typename T>
    size_t process_msg(const net::packet_view& packet);
    void process_msg(const pmd_version* m);
    void process_msg(const pmd_second* m);
    void process_msg(const pmd_order_added* m);
    void process_msg(const pmd_order_executed* m);
    void process_msg(const pmd_order_canceled* m);
    void process_msg(const pmd_order_deleted* m);
    void process_msg(const pmd_broken_trade* m);
    //! Generate a sweep event if execution cleared a price level.
    core::event_mask sweep_event(const core::execution&) const;
    uint64_t to_timestamp(uint64_t nanoseconds) const;
};

}

}
