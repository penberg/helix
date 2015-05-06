#include "helix-c/helix.h"

#include "helix/nasdaq/nordic_itch.hh"
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
	if (!strcmp(name, "nasdaq-nordic-itch")) {
		return wrap(new helix::nasdaq::nordic_itch_protocol{});
	}
	return NULL;
}

helix_session_t
helix_session_create(helix_protocol_t proto, const char *symbol, helix_order_book_callback_t ob_callback, helix_trade_callback_t trade_callback, void *data)
{
	vector<string> symbols;
	symbols.emplace_back(string{symbol});
	return wrap(unwrap(proto)->new_session(symbols, [ob_callback](const helix::core::order_book& ob) {
            ob_callback(wrap(const_cast<helix::core::order_book*>(&ob)));
        }, [trade_callback](const helix::core::trade& trade) {
            trade_callback(wrap(const_cast<helix::core::trade*>(&trade)));
        }, data));
}

void *helix_session_data(helix_session_t session)
{
	return unwrap(session)->data();
}

void helix_session_process_packet(helix_session_t session, const char* buf, size_t len)
{
	unwrap(session)->process_packet(helix::net::packet_view{buf, len});
}

const char *helix_order_book_symbol(helix_order_book_t ob)
{
	return unwrap(ob)->symbol().c_str();
}

uint64_t helix_order_book_timestamp(helix_order_book_t ob)
{
	return unwrap(ob)->timestamp();
}

uint64_t helix_order_book_bid_price(helix_order_book_t ob, size_t level)
{
	return unwrap(ob)->bid_price(level);
}

uint64_t helix_order_book_bid_size(helix_order_book_t ob, size_t level)
{
	return unwrap(ob)->bid_size(level);
}

uint64_t helix_order_book_ask_price(helix_order_book_t ob, size_t level)
{
	return unwrap(ob)->ask_price(level);
}

uint64_t helix_order_book_ask_size(helix_order_book_t ob, size_t level)
{
	return unwrap(ob)->ask_size(level);
}

helix_trading_state_t helix_order_book_state(helix_order_book_t ob)
{
	switch (unwrap(ob)->state()) {
	case helix::core::trading_state::unknown: return HELIX_TRADING_STATE_UNKNOWN;
	case helix::core::trading_state::halted:  return HELIX_TRADING_STATE_HALTED;
	case helix::core::trading_state::trading: return HELIX_TRADING_STATE_TRADING;
	case helix::core::trading_state::auction: return HELIX_TRADING_STATE_AUCTION;
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

uint64_t helix_trade_price(helix_trade_t trade)
{
	return unwrap(trade)->price;
}

helix_trade_sign_t helix_trade_sign(helix_trade_t trade)
{
	switch (unwrap(trade)->sign) {
	case helix::core::trade_sign::buyer_initiated:    return HELIX_TRADE_SIGN_BUYER_INITIATED;
	case helix::core::trade_sign::seller_initiated:   return HELIX_TRADE_SIGN_SELLER_INITIATED;
	case helix::core::trade_sign::crossing:           return HELIX_TRADE_SIGN_CROSSING;
	case helix::core::trade_sign::non_displayable:    return HELIX_TRADE_SIGN_NON_DISPLAYABLE;
	}
        assert(0);
}
