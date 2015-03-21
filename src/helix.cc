#include "helix-c/helix.h"

#include "helix/nasdaq/nordic_itch.h"

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

helix_session_t helix_session_create(helix_protocol_t proto, const char *symbol, helix_callback_t callback)
{
	vector<string> symbols;
	symbols.emplace_back(string{symbol});
	return wrap(unwrap(proto)->new_session(symbols, [callback](const helix::core::order_book& ob) {
            callback(wrap(const_cast<helix::core::order_book*>(&ob)));
        }));
}

void helix_session_process_packet(helix_session_t session, const char* buf, size_t len)
{
	unwrap(session)->process_packet(buf, len);
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
