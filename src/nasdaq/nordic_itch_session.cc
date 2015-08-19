#include "helix/nasdaq/nordic_itch_session.hh"

#include "helix/nasdaq/nordic_itch_handler.hh"
#include "helix/net.hh"
#include "soupfile.hh"
#include "moldudp.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace nasdaq {

nordic_itch_session::nordic_itch_session(shared_ptr<nordic_itch_handler>&&handler,
                                         shared_ptr<net::message_parser>&& transport_session,
                                         void *data)
    : session{data}
    , _handler{std::move(handler)}
    , _transport_session{std::move(transport_session)}
{
}

void nordic_itch_session::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler->subscribe(symbol, max_orders);
}

size_t nordic_itch_session::process_packet(const net::packet_view& packet)
{
    return _transport_session->parse(packet);
}

void nordic_itch_session::register_callback(core::ob_callback process_ob)
{
   _handler->register_callback(process_ob);
}

void nordic_itch_session::register_callback(core::trade_callback process_trade)
{
   _handler->register_callback(process_trade);
}

nordic_itch_session*
nordic_itch_protocol::new_session(void *data)
{
    auto is = make_shared<nordic_itch_handler>();
    shared_ptr<net::message_parser> ts;
    if (_name == "nasdaq-nordic-moldudp-itch") {
        ts = make_shared<moldudp_session>(is);
    } else if (_name == "nasdaq-nordic-soupfile-itch") {
        ts = make_shared<soupfile_session>(is);
    } else {
        throw std::invalid_argument("unknown protocol: " + _name);
    }
    return new nordic_itch_session(std::move(is), std::move(ts), data);
}

}

}
