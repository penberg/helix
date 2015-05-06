/*
 * Copyright (C) 2015 Bitbot LCC
 */

#pragma once

#include "helix/nasdaq/nordic_itch_messages.hh"
#include "helix/core/order_book.hh"
#include "helix/helix.hh"

#include "message_parser.hh"

#include <unordered_map>
#include <vector>
#include <memory>
#include <set>

namespace helix {

namespace nasdaq {

// NASDAQ OXM Nordic ITCH feed.
//
// This is a feed handler for Nordic ITCH. The handler assumes that transport
// protocol framing such as SoapTCP or MoldUDP has already been parsed and
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
class itch_session : public message_parser {
private:
    //! Seconds since midnight in CET (Central European Time).
    uint64_t time_sec;
    //! Milliseconds since @time_sec.
    uint64_t time_msec;
    //! Callback function for processing order book events.
    core::ob_callback _process_ob;
    //! Callback function for processing trade events.
    core::trade_callback _process_trade;
    //! A map of order books by order book ID.
    std::unordered_map<uint64_t, helix::core::order_book> order_book_id_map;
    //! A map of order books by order ID.
    std::unordered_map<uint64_t, helix::core::order_book&> order_id_map;
    //! A set of symbols that we are interested in.
    std::set<std::string> _symbols;
public:
    itch_session(const std::vector<std::string>& symbols, core::ob_callback process_ob, core::trade_callback process_trade)
        : _process_ob{process_ob}
        , _process_trade{process_trade}
    {
        for (auto sym : symbols) {
            auto padding = itch_symbol_len - sym.size();
            if (padding > 0) {
                sym.insert(sym.size(), padding, ' ');
            }
            _symbols.insert(sym);
        }
    }

    virtual void parse(const char* p, size_t size) override;
private:
    //! Timestamp in milliseconds
    inline uint64_t timestamp() const {
        return time_sec * 1000 + time_msec;
    }
};

}

}
