#include "helix-c/Helix.h"

#include "helix/nasdaq/nordic_itch.h"

#include <cassert>
#include <cstring>
#include <string>
#include <vector>

using namespace std;

inline HelixOrderBookRef wrap(helix::core::order_book* ob)
{
	return reinterpret_cast<HelixOrderBookRef>(ob);
}

inline helix::core::order_book* unwrap(HelixOrderBookRef ob)
{
	return reinterpret_cast<helix::core::order_book*>(ob);
}

inline HelixProtocolRef wrap(helix::core::protocol* proto)
{
	return reinterpret_cast<HelixProtocolRef>(proto);
}

inline helix::core::protocol* unwrap(HelixProtocolRef proto)
{
	return reinterpret_cast<helix::core::protocol*>(proto);
}

inline HelixSessionRef wrap(helix::core::session* session)
{
	return reinterpret_cast<HelixSessionRef>(session);
}

inline helix::core::session* unwrap(HelixSessionRef session)
{
	return reinterpret_cast<helix::core::session*>(session);
}

HelixProtocolRef HelixProtocolLookup(const char *name)
{
	if (!strcmp(name, "nasdaq-nordic-itch")) {
		return wrap(new helix::nasdaq::nordic_itch_protocol{});
	}
	return NULL;
}

HelixSessionRef HelixSessionCreate(HelixProtocolRef proto, const char *symbol, HelixCallback callback)
{
	vector<string> symbols;
	symbols.emplace_back(string{symbol});
	return wrap(unwrap(proto)->new_session(symbols, [callback](const helix::core::order_book& ob) {
            callback(wrap(const_cast<helix::core::order_book*>(&ob)));
        }));
}

void HelixSessionProcessPacket(HelixSessionRef session, const char* buf, size_t len)
{
	unwrap(session)->process_packet(buf, len);
}

const char *HelixOrderBookSymbol(HelixOrderBookRef ob)
{
	return unwrap(ob)->symbol().c_str();
}

uint64_t HelixOrderBookTimestamp(HelixOrderBookRef ob)
{
	return unwrap(ob)->timestamp();
}

uint64_t HelixOrderBookBidPrice(HelixOrderBookRef ob, size_t level)
{
	return unwrap(ob)->bid_price(level);
}

uint64_t HelixOrderBookBidSize(HelixOrderBookRef ob, size_t level)
{
	return unwrap(ob)->bid_size(level);
}

uint64_t HelixOrderBookAskPrice(HelixOrderBookRef ob, size_t level)
{
	return unwrap(ob)->ask_price(level);
}

uint64_t HelixOrderBookAskSize(HelixOrderBookRef ob, size_t level)
{
	return unwrap(ob)->ask_size(level);
}

HelixTradingState HelixOrderBookState(HelixOrderBookRef ob)
{
	switch (unwrap(ob)->state()) {
	case helix::core::trading_state::unknown: return HELIX_TRADING_STATE_UNKNOWN;
	case helix::core::trading_state::halted:  return HELIX_TRADING_STATE_HALTED;
	case helix::core::trading_state::trading: return HELIX_TRADING_STATE_TRADING;
	case helix::core::trading_state::auction: return HELIX_TRADING_STATE_AUCTION;
	}
}
