#include "helix/nasdaq/itch50_handler.hh"

#include "helix/order_book.hh"

#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>

#include <endian.h>

using namespace helix::core;
using namespace std;

namespace helix {

namespace nasdaq {

static side_type itch50_side(char c)
{
    switch (c) {
    case 'B': return side_type::buy;
    case 'S': return side_type::sell;
    default:  throw invalid_argument(string("invalid argument: ") + to_string(c));
    }
}

trade_sign itch50_trade_sign(side_type s)
{
    switch (s) {
    case side_type::buy:  return trade_sign::seller_initiated;
    case side_type::sell: return trade_sign::buyer_initiated;
    default:              throw invalid_argument(string("invalid argument"));
    }
}

static uint64_t itch50_timestamp(uint64_t raw_timestamp)
{
    return be64toh(raw_timestamp << 16);
}

size_t itch50_handler::parse(const net::packet_view& packet)
{
    auto* msg = packet.cast<itch50_message>();
    switch (msg->MessageType) {
    case 'S': return process_msg<itch50_system_event>(packet);
    case 'R': return process_msg<itch50_stock_directory>(packet);
    case 'H': return process_msg<itch50_stock_trading_action>(packet);
    case 'Y': return process_msg<itch50_reg_sho_restriction>(packet);
    case 'L': return process_msg<itch50_market_participant_position>(packet);
    case 'V': return process_msg<itch50_mwcb_decline_level>(packet);
    case 'W': return process_msg<itch50_mwcb_breach>(packet);
    case 'K': return process_msg<itch50_ipo_quoting_period_update>(packet);
    case 'A': return process_msg<itch50_add_order>(packet);
    case 'F': return process_msg<itch50_add_order_mpid>(packet);
    case 'E': return process_msg<itch50_order_executed>(packet);
    case 'C': return process_msg<itch50_order_executed_with_price>(packet);
    case 'X': return process_msg<itch50_order_cancel>(packet);
    case 'D': return process_msg<itch50_order_delete>(packet);
    case 'U': return process_msg<itch50_order_replace>(packet);
    case 'P': return process_msg<itch50_trade>(packet);
    case 'Q': return process_msg<itch50_cross_trade>(packet);
    case 'B': return process_msg<itch50_broken_trade>(packet);
    case 'I': return process_msg<itch50_noii>(packet);
    case 'N': return process_msg<itch50_rpii>(packet);
    default:  throw unknown_message_type("unknown type: " + std::string(1, msg->MessageType));
    }
}

template<typename T>
size_t itch50_handler::process_msg(const net::packet_view& packet)
{
    process_msg(packet.cast<T>());
    return sizeof(T);
}

void itch50_handler::process_msg(const itch50_system_event* m)
{
}

void itch50_handler::process_msg(const itch50_stock_directory* m)
{
    std::string sym{m->Stock, ITCH_SYMBOL_LEN};
    if (_symbols.count(sym) > 0) {
        order_book ob{sym, itch50_timestamp(m->Timestamp), _symbol_max_orders.at(sym)};
        order_book_id_map.insert({m->StockLocate, std::move(ob)});
    }
}

void itch50_handler::process_msg(const itch50_stock_trading_action* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        auto& ob = it->second;

        switch (m->TradingState) {
        case 'H': ob.set_state(trading_state::halted); break;
        case 'P': ob.set_state(trading_state::paused); break;
        case 'Q': ob.set_state(trading_state::quotation_only); break;
        case 'T': ob.set_state(trading_state::trading); break;
        default:  throw invalid_argument(string("invalid trading state: ") + to_string(m->TradingState));
        }
    }
}

void itch50_handler::process_msg(const itch50_reg_sho_restriction* m)
{
}

void itch50_handler::process_msg(const itch50_market_participant_position* m)
{
}

void itch50_handler::process_msg(const itch50_mwcb_decline_level* m)
{
}

void itch50_handler::process_msg(const itch50_mwcb_breach* m)
{
}

void itch50_handler::process_msg(const itch50_ipo_quoting_period_update* m)
{
}

void itch50_handler::process_msg(const itch50_add_order* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        auto& ob = it->second;

        uint64_t order_id = m->OrderReferenceNumber;
        uint64_t price    = be32toh(m->Price);
        uint32_t quantity = be32toh(m->Shares);
        auto     side     = itch50_side(m->BuySellIndicator);
        uint64_t timestamp = itch50_timestamp(m->Timestamp);
        order o{order_id, price, quantity, side, timestamp};
        ob.add(std::move(o));
        ob.set_timestamp(timestamp);
        _process_ob(ob);
    }
}

void itch50_handler::process_msg(const itch50_add_order_mpid* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        auto& ob = it->second;

        uint64_t order_id = m->OrderReferenceNumber;
        uint64_t price    = be32toh(m->Price);
        uint32_t quantity = be32toh(m->Shares);
        auto     side     = itch50_side(m->BuySellIndicator);
        uint64_t timestamp = itch50_timestamp(m->Timestamp);
        order o{order_id, price, quantity, side, timestamp};
        ob.add(std::move(o));
        ob.set_timestamp(timestamp);
        _process_ob(ob);
    }
}

void itch50_handler::process_msg(const itch50_order_executed* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        uint64_t quantity = be32toh(m->ExecutedShares);
        uint64_t timestamp = itch50_timestamp(m->Timestamp);
        auto& ob = it->second;
        auto result = ob.execute(m->OrderReferenceNumber, quantity);
        ob.set_timestamp(timestamp);
        _process_ob(ob);
        _process_trade(trade{ob.symbol(), timestamp, result.first, quantity, itch50_trade_sign(result.second)});
    }
}

void itch50_handler::process_msg(const itch50_order_executed_with_price* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        uint64_t quantity = be32toh(m->ExecutedShares);
        uint64_t price = be32toh(m->ExecutionPrice);
        uint64_t timestamp = itch50_timestamp(m->Timestamp);
        auto& ob = it->second;
        auto result = ob.execute(m->OrderReferenceNumber, quantity);
        ob.set_timestamp(timestamp);
        _process_ob(ob);
        _process_trade(trade{ob.symbol(), timestamp, price, quantity, itch50_trade_sign(result.second)});
    }
}

void itch50_handler::process_msg(const itch50_order_cancel* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        auto& ob = it->second;
        ob.cancel(m->OrderReferenceNumber, be32toh(m->CanceledShares));
        ob.set_timestamp(itch50_timestamp(m->Timestamp));
        _process_ob(ob);
    }

}

void itch50_handler::process_msg(const itch50_order_delete* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        auto& ob = it->second;
        ob.remove(m->OrderReferenceNumber);
        ob.set_timestamp(itch50_timestamp(m->Timestamp));
        _process_ob(ob);
    }

}

void itch50_handler::process_msg(const itch50_order_replace* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        auto& ob = it->second;

        auto side = ob.side(m->OriginalOrderReferenceNumber);

        ob.remove(m->OriginalOrderReferenceNumber);

        uint64_t order_id = m->NewOrderReferenceNumber;
        uint64_t price    = be32toh(m->Price);
        uint32_t quantity = be32toh(m->Shares);
        uint64_t timestamp = itch50_timestamp(m->Timestamp);
        order o{order_id, price, quantity, side, timestamp};
        ob.add(std::move(o));
        ob.set_timestamp(timestamp);
        _process_ob(ob);
    }
}

void itch50_handler::process_msg(const itch50_trade* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        uint64_t trade_price = be32toh(m->Price);
        uint32_t quantity = be32toh(m->Shares);
        auto& ob = it->second;
        _process_trade(trade{ob.symbol(), itch50_timestamp(m->Timestamp), trade_price, quantity, trade_sign::non_displayable});
    }
}

void itch50_handler::process_msg(const itch50_cross_trade* m)
{
    auto it = order_book_id_map.find(m->StockLocate);
    if (it != order_book_id_map.end()) {
        uint64_t cross_price = be32toh(m->CrossPrice);
        uint64_t quantity = be64toh(m->Shares);
        auto& ob = it->second;
        _process_trade(trade{ob.symbol(), itch50_timestamp(m->Timestamp), cross_price, quantity, trade_sign::crossing});
    }
}

void itch50_handler::process_msg(const itch50_broken_trade* m)
{
}

void itch50_handler::process_msg(const itch50_noii* m)
{
}

void itch50_handler::process_msg(const itch50_rpii* m)
{
}

}

}
