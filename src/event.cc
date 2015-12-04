#include "helix/helix.hh"

namespace helix {

namespace core {

event::event(event_mask mask, order_book* ob, trade* t)
    : _mask{mask}
    , _ob{ob}
    , _trade{t}
{
}

event_mask event::get_mask() const
{
    return _mask;
}

order_book* event::get_ob() const
{
    return _ob;
}

trade* event::get_trade() const
{
    return _trade;
}

event make_event(order_book* ob, trade* t, event_mask mask)
{
    return event{mask | ev_order_book_update | ev_trade, ob, t};
}

event make_ob_event(order_book* ob, event_mask mask)
{
    return event{mask | ev_order_book_update, ob, nullptr};
}

event make_trade_event(trade* t, event_mask mask)
{
    return event{mask | ev_trade, nullptr, t};
}

}

}
