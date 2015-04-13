#include "itch_session.hh"

#include "helix/core/order_book.hh"
#include "itch_messages.hh"

#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

using namespace helix::core;
using namespace std;

namespace helix {

namespace nasdaq {

static side itch_side(char c)
{
    switch (c) {
    case 'B': return side::buy;
    case 'S': return side::sell;
    default:  throw invalid_argument(string("invalid argument: ") + to_string(c));
    }
}

trade_sign itch_trade_sign(side s)
{
    switch (s) {
    case side::buy:  return trade_sign::seller_initiated;
    case side::sell: return trade_sign::buyer_initiated;
    default:         throw invalid_argument(string("invalid argument"));
    }
}

void itch_session::parse(const char* buf, size_t len)
{
    auto* msg = reinterpret_cast<const itch_message*>(buf);

    switch (msg->MsgType) {
    case 'T': {
        auto m = reinterpret_cast<const itch_seconds*>(buf);
        auto second = itch_uatoi(m->Second, 5);
        time_sec = second;
        break;
    }
    case 'M': {
        auto m = reinterpret_cast<const itch_milliseconds*>(buf);
        auto millisecond = itch_uatoi(m->Millisecond, 3);
        time_msec = millisecond;
        break;
    }
    case 'R': {
        auto m = reinterpret_cast<const itch_order_book_directory*>(buf);
        auto order_book_id = itch_uatoi(m->OrderBook, sizeof(m->OrderBook));

        std::string sym{m->Symbol, itch_symbol_len};
        if (_symbols.count(sym) > 0) {
            order_book ob{sym, timestamp()};
            order_book_id_map.insert({order_book_id, std::move(ob)});
        }
        break;
    }
    case 'H': {
        auto m = reinterpret_cast<const itch_order_book_trading_action*>(buf);
        auto order_book_id = itch_uatoi(m->OrderBook, sizeof(m->OrderBook));

        auto it = order_book_id_map.find(order_book_id);
        if (it != order_book_id_map.end()) {
            auto& ob = it->second;

            switch (m->TradingState) {
            case 'H': ob.set_state(trading_state::halted ); break;
            case 'T': ob.set_state(trading_state::trading); break;
            case 'Q': ob.set_state(trading_state::auction); break;
            default : throw invalid_argument(string("invalid trading state: ") + to_string(m->TradingState));
            }
            ob.set_timestamp(timestamp());
        }
        break;
    }
    case 'A': {
        auto m = reinterpret_cast<const itch_add_order*>(buf);
        auto order_book_id = itch_uatoi(m->OrderBook, sizeof(m->OrderBook));

        auto it = order_book_id_map.find(order_book_id);
        if (it != order_book_id_map.end()) {
            auto& ob = it->second;

            uint64_t order_id = itch_uatoi(m->OrderReferenceNumber, sizeof(m->OrderReferenceNumber));
            uint64_t price    = itch_uatoi(m->Price, sizeof(m->Price));;
            uint64_t quantity = itch_uatoi(m->Quantity, sizeof(m->Quantity));;
            auto     side     = itch_side(m->BuySellIndicator);

            order o{order_id, price, quantity, side};
            ob.add(std::move(o));

            order_id_map.insert({order_id, ob});
            ob.set_timestamp(timestamp());
            _process_ob(ob);
        }
        break;
    }
    case 'F': {
        auto m = reinterpret_cast<const itch_add_order_mpid*>(buf);
        auto order_book_id = itch_uatoi(m->OrderBook, sizeof(m->OrderBook));

        auto it = order_book_id_map.find(order_book_id);
        if (it != order_book_id_map.end()) {
            auto& ob = it->second;

            uint64_t order_id = itch_uatoi(m->OrderReferenceNumber, sizeof(m->OrderReferenceNumber));
            uint64_t price    = itch_uatoi(m->Price, sizeof(m->Price));;
            uint64_t quantity = itch_uatoi(m->Quantity, sizeof(m->Quantity));;
            auto     side     = itch_side(m->BuySellIndicator);

            order o{order_id, price, quantity, side};
            ob.add(std::move(o));

            order_id_map.insert({order_id, ob});
            ob.set_timestamp(timestamp());
            _process_ob(ob);
        }
        break;
    }
    case 'E': {
        auto m = reinterpret_cast<const itch_order_executed*>(buf);
        uint64_t order_id = itch_uatoi(m->OrderReferenceNumber, sizeof(m->OrderReferenceNumber));
        uint64_t quantity = itch_uatoi(m->ExecutedQuantity, sizeof(m->ExecutedQuantity));

        auto it = order_id_map.find(order_id);
        if (it != order_id_map.end()) {
            auto& ob = it->second;
            auto result = ob.execute(order_id, quantity);
            ob.set_timestamp(timestamp());
            _process_ob(ob);
            _process_trade(trade{ob.symbol(), timestamp(), result.first, itch_trade_sign(result.second)});
        }
        break;
    }
    case 'C': {
        auto m = reinterpret_cast<const itch_order_executed_with_price*>(buf);
        uint64_t order_id = itch_uatoi(m->OrderReferenceNumber, sizeof(m->OrderReferenceNumber));
        uint64_t quantity = itch_uatoi(m->ExecutedQuantity, sizeof(m->ExecutedQuantity));
        uint64_t price = itch_uatoi(m->TradePrice, sizeof(m->TradePrice));

        auto it = order_id_map.find(order_id);
        if (it != order_id_map.end()) {
            auto& ob = it->second;
            auto result = ob.execute(order_id, quantity);
            ob.set_timestamp(timestamp());
            _process_ob(ob);
            _process_trade(trade{ob.symbol(), timestamp(), price, itch_trade_sign(result.second)});
        }
        break;
    }
    case 'X': {
        auto m = reinterpret_cast<const itch_order_cancel*>(buf);
        uint64_t order_id = itch_uatoi(m->OrderReferenceNumber, sizeof(m->OrderReferenceNumber));
        uint64_t quantity = itch_uatoi(m->CanceledQuantity, sizeof(m->CanceledQuantity));

        auto it = order_id_map.find(order_id);
        if (it != order_id_map.end()) {
            auto& ob = it->second;
            ob.cancel(order_id, quantity);
            ob.set_timestamp(timestamp());
            _process_ob(ob);
        }
        break;
    }
    case 'D': {
        auto m = reinterpret_cast<const itch_order_delete*>(buf);
        uint64_t order_id = itch_uatoi(m->OrderReferenceNumber, sizeof(m->OrderReferenceNumber));

        auto it = order_id_map.find(order_id);
        if (it != order_id_map.end()) {
            auto& ob = it->second;
            ob.remove(order_id);
            ob.set_timestamp(timestamp());
            _process_ob(ob);
        }
        break;
    }
    case 'P': {
        auto m = reinterpret_cast<const itch_trade*>(buf);
        auto order_book_id = itch_uatoi(m->OrderBook, sizeof(m->OrderBook));
        uint64_t trade_price = itch_uatoi(m->TradePrice, sizeof(m->TradePrice));

        auto it = order_book_id_map.find(order_book_id);
        if (it != order_book_id_map.end()) {
            auto& ob = it->second;
            _process_trade(trade{ob.symbol(), timestamp(), trade_price, trade_sign::non_displayable});
        }
        break;
    }
    case 'Q': {
        auto m = reinterpret_cast<const itch_cross_trade*>(buf);
        auto order_book_id = itch_uatoi(m->OrderBook, sizeof(m->OrderBook));
        uint64_t cross_price = itch_uatoi(m->CrossPrice, sizeof(m->CrossPrice));

        auto it = order_book_id_map.find(order_book_id);
        if (it != order_book_id_map.end()) {
            auto& ob = it->second;
            _process_trade(trade{ob.symbol(), timestamp(), cross_price, trade_sign::crossing});
        }
        break;
    }
    default:
        break;
    }
}

}

}
