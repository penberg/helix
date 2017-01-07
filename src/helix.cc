#include "helix-c/helix.h"

#include "helix/nasdaq/nordic_itch_protocol.hh"
#include "helix/nasdaq/itch50_protocol.hh"
#include "helix/parity/pmd_protocol.hh"
#include "helix/net.hh"

inline helix_order_book_t wrap(helix::order_book* ob)
{
    return reinterpret_cast<helix_order_book_t>(ob);
}

inline helix::order_book* unwrap(helix_order_book_t ob)
{
    return reinterpret_cast<helix::order_book*>(ob);
}

inline helix_trade_t wrap(helix::trade* ob)
{
    return reinterpret_cast<helix_trade_t>(ob);
}

inline helix::trade* unwrap(helix_trade_t ob)
{
    return reinterpret_cast<helix::trade*>(ob);
}

inline helix_event_t wrap(helix::event* ob)
{
    return reinterpret_cast<helix_event_t>(ob);
}

inline helix::event* unwrap(helix_event_t ob)
{
    return reinterpret_cast<helix::event*>(ob);
}

inline helix_protocol_t wrap(helix::protocol* proto)
{
    return reinterpret_cast<helix_protocol_t>(proto);
}

inline helix::protocol* unwrap(helix_protocol_t proto)
{
    return reinterpret_cast<helix::protocol*>(proto);
}

inline helix_session_t wrap(helix::session* session)
{
    return reinterpret_cast<helix_session_t>(session);
}

inline helix::session* unwrap(helix_session_t session)
{
    return reinterpret_cast<helix::session*>(session);
}

const char *helix_strerror(int error)
{
    switch (error) {
    case HELIX_ERROR_UNKNOWN_MESSAGE_TYPE: return "unknown message type";
    case HELIX_ERROR_TRUNCATED_PACKET: return "truncated packet";
    case HELIX_ERROR_UNKNOWN: return "unknown error";
    default: return "invalid error";
    }
}

helix_protocol_t helix_protocol_lookup(const char *name)
{
    if (helix::nasdaq::nordic_itch_protocol::supports(name)) {
        return wrap(new helix::nasdaq::nordic_itch_protocol{name});
    }
    if (helix::nasdaq::itch50_protocol::supports(name)) {
        return wrap(new helix::nasdaq::itch50_protocol{name});
    }
    if (helix::parity::pmd_protocol::supports(name)) {
        return wrap(new helix::parity::pmd_protocol{name});
    }
    return NULL;
}

void helix_protocol_destroy(helix_protocol_t proto)
{
    delete unwrap(proto);
}

helix_session_t
helix_session_create(helix_protocol_t proto, helix_event_callback_t callback, void *data)
{
    auto session = unwrap(proto)->new_session(data);
    session->register_callback([session, callback](const helix::event& event) {
        callback(wrap(session), wrap(const_cast<helix::event*>(&event)));
    });
    return wrap(session);
}

void helix_session_destroy(helix_session_t session)
{
    delete unwrap(session);
}

void helix_session_subscribe(helix_session_t session, const char *symbol, size_t max_orders)
{
    unwrap(session)->subscribe(symbol, max_orders);
}

void helix_session_set_send_callback(helix_session_t session, helix_send_callback_t callback)
{
    unwrap(session)->set_send_callback([session, callback](char* base, size_t len) {
        callback(session, base, len);
    });
}

void *helix_session_data(helix_session_t session)
{
    return unwrap(session)->data();
}

bool helix_session_is_rth_timestamp(helix_session_t session, helix_timestamp_t timestamp)
{
    return unwrap(session)->is_rth_timestamp(timestamp);
}

int helix_session_process_packet(helix_session_t session, const char* buf, size_t len)
{
    try {
        return unwrap(session)->process_packet(helix::net::packet_view{buf, len});
    } catch (const helix::unknown_message_type& e) {
        return HELIX_ERROR_UNKNOWN_MESSAGE_TYPE;
    } catch (const helix::truncated_packet_error& e) {
        return HELIX_ERROR_TRUNCATED_PACKET;
    } catch (...) {
       return HELIX_ERROR_UNKNOWN;
    }
}

helix_event_mask_t helix_event_mask(helix_event_t ev)
{
    return static_cast<helix_event_mask_t>(unwrap(ev)->get_mask());
}

const char *helix_event_symbol(helix_event_t ev)
{
    return unwrap(ev)->get_symbol().c_str();
}

helix_timestamp_t helix_event_timestamp(helix_event_t ev)
{
    return unwrap(ev)->get_timestamp();
}

helix_order_book_t helix_event_order_book(helix_event_t ev)
{
    return wrap(unwrap(ev)->get_ob());
}

helix_trade_t helix_event_trade(helix_event_t ev)
{
    return wrap(unwrap(ev)->get_trade());
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
    using namespace helix;
    switch (unwrap(ob)->state()) {
    case trading_state::unknown:        return HELIX_TRADING_STATE_UNKNOWN;
    case trading_state::halted:         return HELIX_TRADING_STATE_HALTED;
    case trading_state::paused:         return HELIX_TRADING_STATE_PAUSED;
    case trading_state::quotation_only: return HELIX_TRADING_STATE_QUOTATION_ONLY;
    case trading_state::trading:        return HELIX_TRADING_STATE_TRADING;
    case trading_state::auction:        return HELIX_TRADING_STATE_AUCTION;
    }
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
    case helix::trade_sign::buyer_initiated:  return HELIX_TRADE_SIGN_BUYER_INITIATED;
    case helix::trade_sign::seller_initiated: return HELIX_TRADE_SIGN_SELLER_INITIATED;
    case helix::trade_sign::crossing:         return HELIX_TRADE_SIGN_CROSSING;
    case helix::trade_sign::non_displayable:  return HELIX_TRADE_SIGN_NON_DISPLAYABLE;
    }
}
