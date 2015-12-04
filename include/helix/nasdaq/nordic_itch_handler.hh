#pragma once

#include "helix/nasdaq/nordic_itch_messages.h"
#include "helix/order_book.hh"
#include "helix/helix.hh"
#include "helix/net.hh"

#include <unordered_map>
#include <vector>
#include <memory>
#include <set>

namespace helix {

namespace nasdaq {

// NASDAQ OXM Nordic ITCH feed.
//
// This is a feed handler for Nordic ITCH. The handler assumes that transport
// protocol framing such as SoupTCP or MoldUDP has already been parsed and
// works directly on ITCH messages.
//
// The feed handler reconstructs a full depth order book from a ITCH message
// flow using an algorithm that is specified in Appendix A of the protocol
// specification. As not all messages include an order book ID, the handler
// keeps mapping from every order ID it encounters to the order book the order
// is part of.
//
// The ITCH variant processed by this feed handler is specified by NASDAQ OMX
// in:
//
//   Nordic Equity TotalView-ITCH
//   Version 1.90.2
//   April 7, 2014
//
class nordic_itch_handler : public net::message_parser {
    //! Seconds since midnight in CET (Central European Time).
    uint64_t time_sec;
    //! Milliseconds since @time_sec.
    uint64_t time_msec;
    //! Callback function for processing events.
    core::event_callback _process_event;
    //! A map of order books by order book ID.
    std::unordered_map<uint64_t, helix::core::order_book> order_book_id_map;
    //! A map of order books by order ID.
    std::unordered_map<uint64_t, helix::core::order_book&> order_id_map;
    //! A set of symbols that we are interested in.
    std::set<std::string> _symbols;
    //! A map of pre-allocation size by symbol.
    std::unordered_map<std::string, size_t> _symbol_max_orders;
public:
    class unknown_message_type : public std::logic_error {
    public:
        explicit unknown_message_type(std::string cause)
            : logic_error(std::move(cause))
        { }
    };
    nordic_itch_handler();
    void subscribe(std::string sym, size_t max_orders);
    void register_callback(core::event_callback callback);
    virtual size_t parse(const net::packet_view& packet) override;
private:
    template<typename T>
    size_t process_msg(const net::packet_view& packet);
    void process_msg(const itch_seconds* m);
    void process_msg(const itch_milliseconds* m);
    void process_msg(const itch_market_segment_state* m);
    void process_msg(const itch_system_event* m);
    void process_msg(const itch_order_book_directory* m);
    void process_msg(const itch_order_book_trading_action* m);
    void process_msg(const itch_add_order* m);
    void process_msg(const itch_add_order_mpid* m);
    void process_msg(const itch_order_executed* m);
    void process_msg(const itch_order_executed_with_price* m);
    void process_msg(const itch_order_cancel* m);
    void process_msg(const itch_order_delete* m);
    void process_msg(const itch_trade* m);
    void process_msg(const itch_cross_trade* m);
    void process_msg(const itch_broken_trade* m);
    void process_msg(const itch_noii* m);
    //! Generate a sweep event if execution cleared a price level.
    core::event_mask sweep_event(const core::execution&) const;
    //! Timestamp in milliseconds
    uint64_t timestamp() const;
};

}

}
