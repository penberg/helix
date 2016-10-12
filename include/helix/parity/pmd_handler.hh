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
    event_callback _process_event;
    //! A map of order books by order book ID.
    std::unordered_map<std::string, helix::order_book> _order_book_id_map;
    //! A map of order books by order ID.
    std::unordered_map<uint64_t, helix::order_book&> _order_id_map;
    //! A set of symbols that we are interested in.
    std::set<std::string> _symbols;
    //! Number of seconds since midnight when the trading session started.
    uint32_t _seconds;
public:
    pmd_handler();
    bool is_rth_timestamp(uint64_t timestamp) const;
    void subscribe(std::string sym, size_t max_orders);
    void register_callback(event_callback callback);
    size_t process_packet(const net::packet_view& packet, bool sync);
private:
    template<typename T>
    size_t process_msg(const net::packet_view& packet, bool sync);
    void process_msg(const pmd_version* m, bool sync);
    void process_msg(const pmd_second* m, bool sync);
    void process_msg(const pmd_order_added* m, bool sync);
    void process_msg(const pmd_order_executed* m, bool sync);
    void process_msg(const pmd_order_canceled* m, bool sync);
    void process_msg(const pmd_order_deleted* m, bool sync);
    void process_msg(const pmd_broken_trade* m, bool sync);
    //! Generate a sweep event if execution cleared a price level.
    event_mask sweep_event(const execution&) const;
    uint64_t to_timestamp(uint64_t nanoseconds) const;
};

}

}
