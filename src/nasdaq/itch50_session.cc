#include "helix/nasdaq/itch50_session.hh"

#include "helix/nasdaq/itch50_handler.hh"
#include "binaryfile.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace nasdaq {

itch50_session::itch50_session(shared_ptr<itch50_handler>&&handler,
                             shared_ptr<net::message_parser>&& transport_session,
                             void *data)
    : session{data}
    , _handler{std::move(handler)}
    , _transport_session{std::move(transport_session)}
{
}

void itch50_session::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler->subscribe(symbol, max_orders);
}

size_t itch50_session::process_packet(const net::packet_view& packet)
{
    return _transport_session->parse(packet);
}

void itch50_session::register_callback(core::ob_callback process_ob)
{
   _handler->register_callback(process_ob);
}

void itch50_session::register_callback(core::trade_callback process_trade)
{
   _handler->register_callback(process_trade);
}

itch50_session*
itch50_protocol::new_session(void *data)
{
    auto is = make_shared<itch50_handler>();
    shared_ptr<net::message_parser> ts;
    if (_name == "nasdaq-binaryfile-itch50") {
        ts = make_shared<binaryfile_session>(is);
    } else {
        throw std::invalid_argument("unknown protocol: " + _name);
    }
    return new itch50_session(std::move(is), std::move(ts), data);
}

}

}
