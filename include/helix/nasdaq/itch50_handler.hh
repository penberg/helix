#pragma once

#include "helix/nasdaq/itch50_messages.h"
#include "helix/order_book.hh"
#include "helix/helix.hh"
#include "helix/net.hh"

#include <unordered_map>
#include <vector>
#include <memory>
#include <set>

namespace helix {

namespace nasdaq {

// NASDAQ TotalView-ITCH 5.0
//
// This is a feed handler for Total-View ITCH. The handler assumes that
// transport protocol framing such as SoupTCP or MoldUDP has already been
// parsed and works directly on ITCH messages.
//
// The ITCH variant processed by this feed handler is specified by NASDAQ in:
//
//   NASDAQ TotalView-ITCH 5.0
//   Version 5.0
//   03/06/2015
//
class itch50_handler : public net::message_parser {
private:
    //! Callback function for processing events.
    core::event_callback _process_event;
    //! A map of order books by order book ID.
    std::unordered_map<uint64_t, helix::core::order_book> order_book_id_map;
    //! A set of symbols that we are interested in.
    std::set<std::string> _symbols;
    //! A map of pre-allocation size by symbol.
    std::unordered_map<std::string, size_t> _symbol_max_orders;
public:
    class unknown_message_type : public std::logic_error {
    public:
        unknown_message_type(std::string cause)
            : logic_error(std::move(cause))
        { }
    };
public:
    itch50_handler()
    { }
    void subscribe(std::string sym, size_t max_orders) {
        auto padding = ITCH_SYMBOL_LEN - sym.size();
        if (padding > 0) {
            sym.insert(sym.size(), padding, ' ');
        }
        _symbols.insert(sym);
        _symbol_max_orders.emplace(sym, max_orders);
        size_t max_all_orders = 0;
        for (auto&& kv : _symbol_max_orders) {
            max_all_orders += kv.second;
        }
        order_book_id_map.reserve(max_all_orders);
    }
    void register_callback(core::event_callback callback) {
        _process_event = callback;
    }
    virtual size_t parse(const net::packet_view& packet) override;
private:
    template<typename T>
    size_t process_msg(const net::packet_view& packet);
    void process_msg(const itch50_system_event* m);
    void process_msg(const itch50_stock_directory* m);
    void process_msg(const itch50_stock_trading_action* m);
    void process_msg(const itch50_reg_sho_restriction* m);
    void process_msg(const itch50_market_participant_position* m);
    void process_msg(const itch50_mwcb_decline_level* m);
    void process_msg(const itch50_mwcb_breach* m);
    void process_msg(const itch50_ipo_quoting_period_update* m);
    void process_msg(const itch50_add_order* m);
    void process_msg(const itch50_add_order_mpid* m);
    void process_msg(const itch50_order_executed* m);
    void process_msg(const itch50_order_executed_with_price* m);
    void process_msg(const itch50_order_cancel* m);
    void process_msg(const itch50_order_delete* m);
    void process_msg(const itch50_order_replace* m);
    void process_msg(const itch50_trade* m);
    void process_msg(const itch50_cross_trade* m);
    void process_msg(const itch50_broken_trade* m);
    void process_msg(const itch50_noii* m);
    void process_msg(const itch50_rpii* m);
};

}

}
