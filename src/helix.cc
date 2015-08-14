#include "helix-c/helix.h"

#include "helix/nasdaq/nordic_itch.hh"
#include "helix/nasdaq/itch50.hh"
#include "helix/net.hh"

#include <cassert>
#include <cstring>
#include <string>
#include <vector>

using namespace std;

inline helix_order_book_t wrap(helix::core::order_book* ob)
{
    return reinterpret_cast<helix_order_book_t>(ob);
}

inline helix::core::order_book* unwrap(helix_order_book_t ob)
{
    return reinterpret_cast<helix::core::order_book*>(ob);
}

inline helix_trade_t wrap(helix::core::trade* ob)
{
    return reinterpret_cast<helix_trade_t>(ob);
}

inline helix::core::trade* unwrap(helix_trade_t ob)
{
    return reinterpret_cast<helix::core::trade*>(ob);
}

inline helix_protocol_t wrap(helix::core::protocol* proto)
{
    return reinterpret_cast<helix_protocol_t>(proto);
}

inline helix::core::protocol* unwrap(helix_protocol_t proto)
{
    return reinterpret_cast<helix::core::protocol*>(proto);
}

inline helix_session_t wrap(helix::core::session* session)
{
    return reinterpret_cast<helix_session_t>(session);
}

inline helix::core::session* unwrap(helix_session_t session)
{
    return reinterpret_cast<helix::core::session*>(session);
}

helix_protocol_t helix_protocol_lookup(const char *name)
{
    if (!strcmp(name, "nasdaq-nordic-moldudp-itch") || !strcmp(name, "nasdaq-nordic-soupfile-itch")) {
        return wrap(new helix::nasdaq::nordic_itch_protocol{name});
    }
    if (!strcmp(name, "nasdaq-binaryfile-itch50")) {
        return wrap(new helix::nasdaq::itch50_protocol{name});
    }
    return NULL;
}

helix_session_t
helix_session_create(helix_protocol_t proto, helix_order_book_callback_t ob_callback, helix_trade_callback_t trade_callback, void *data)
{
    auto session = unwrap(proto)->new_session(data);
    session->register_callback([session, ob_callback](const helix::core::order_book& ob) {
        ob_callback(wrap(session), wrap(const_cast<helix::core::order_book*>(&ob)));
    });
    session->register_callback([session, trade_callback](const helix::core::trade& trade) {
        trade_callback(wrap(session), wrap(const_cast<helix::core::trade*>(&trade)));
    });
    return wrap(session);
}

void helix_session_subscribe(helix_session_t session, const char *symbol)
{
    unwrap(session)->subscribe(symbol);
}

void *helix_session_data(helix_session_t session)
{
    return unwrap(session)->data();
}

size_t helix_session_process_packet(helix_session_t session, const char* buf, size_t len)
{
    return unwrap(session)->process_packet(helix::net::packet_view{buf, len});
}

const char *helix_order_book_symbol(helix_order_book_t ob)
{
    return unwrap(ob)->symbol().c_str();
}

helix_timestamp_t helix_order_book_timestamp(helix_order_book_t ob)
{
    return unwrap(ob)->timestamp();
}

size_t helix_order_book_bid_levels(helix_order_book_t ob)
{
    return unwrap(ob)->bid_levels();
}

size_t helix_order_book_ask_levels(helix_order_book_t ob)
{
    return unwrap(ob)->ask_levels();
}

size_t helix_order_book_order_count(helix_order_book_t ob)
{
    return unwrap(ob)->order_count();
}

helix_price_t helix_order_book_bid_price(helix_order_book_t ob, size_t level)
{
    return unwrap(ob)->bid_price(level);
}

uint64_t helix_order_book_bid_size(helix_order_book_t ob, size_t level)
{
    return unwrap(ob)->bid_size(level);
}

helix_price_t helix_order_book_ask_price(helix_order_book_t ob, size_t level)
{
    return unwrap(ob)->ask_price(level);
}

uint64_t helix_order_book_ask_size(helix_order_book_t ob, size_t level)
{
    return unwrap(ob)->ask_size(level);
}

helix_price_t helix_order_book_midprice(helix_order_book_t ob, size_t level)
{
    return unwrap(ob)->midprice(level);
}

helix_trading_state_t helix_order_book_state(helix_order_book_t ob)
{
    using namespace helix::core;
    switch (unwrap(ob)->state()) {
    case trading_state::unknown:        return HELIX_TRADING_STATE_UNKNOWN;
    case trading_state::halted:         return HELIX_TRADING_STATE_HALTED;
    case trading_state::paused:         return HELIX_TRADING_STATE_PAUSED;
    case trading_state::quotation_only: return HELIX_TRADING_STATE_QUOTATION_ONLY;
    case trading_state::trading:        return HELIX_TRADING_STATE_TRADING;
    case trading_state::auction:        return HELIX_TRADING_STATE_AUCTION;
    }
    assert(0);
}

const char *helix_trade_symbol(helix_trade_t trade)
{
    return unwrap(trade)->symbol.c_str();
}

uint64_t helix_trade_timestamp(helix_trade_t trade)
{
    return unwrap(trade)->timestamp;
}

helix_price_t helix_trade_price(helix_trade_t trade)
{
    return unwrap(trade)->price;
}

uint64_t helix_trade_size(helix_trade_t trade)
{
    return unwrap(trade)->size;
}

helix_trade_sign_t helix_trade_sign(helix_trade_t trade)
{
    switch (unwrap(trade)->sign) {
    case helix::core::trade_sign::buyer_initiated:  return HELIX_TRADE_SIGN_BUYER_INITIATED;
    case helix::core::trade_sign::seller_initiated: return HELIX_TRADE_SIGN_SELLER_INITIATED;
    case helix::core::trade_sign::crossing:         return HELIX_TRADE_SIGN_CROSSING;
    case helix::core::trade_sign::non_displayable:  return HELIX_TRADE_SIGN_NON_DISPLAYABLE;
    }
    assert(0);
}
