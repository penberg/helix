#include "helix/helix.hh"

namespace helix {

event::event(event_mask mask, const std::string& symbol, uint64_t timestamp, order_book* ob, trade* t)
    : _mask{mask}
    , _symbol{symbol}
    , _timestamp{timestamp}
    , _ob{ob}
    , _trade{t}
{
}

event_mask event::get_mask() const
{
    return _mask;
}

const std::string& event::get_symbol() const
{
    return _symbol;
}

uint64_t event::get_timestamp() const
{
    return _timestamp;
}

order_book* event::get_ob() const
{
    return _ob;
}

trade* event::get_trade() const
{
    return _trade;
}

event make_event(const std::string& symbol, uint64_t timestamp, order_book* ob, trade* t, event_mask mask)
{
    return event{mask | ev_order_book_update | ev_trade, symbol, timestamp, ob, t};
}

event make_ob_event(const std::string& symbol, uint64_t timestamp, order_book* ob, event_mask mask)
{
    return event{mask | ev_order_book_update, symbol, timestamp, ob, nullptr};
}

event make_trade_event(const std::string& symbol, uint64_t timestamp, trade* t, event_mask mask)
{
    return event{mask | ev_trade, symbol, timestamp, nullptr, t};
}

}
