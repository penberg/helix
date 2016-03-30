#include "helix/nasdaq/nordic_itch_session.hh"

#include "helix/nasdaq/nordic_itch_handler.hh"
#include "helix/net.hh"
#include "soupfile.hh"
#include "moldudp.hh"

#include <chrono>
#include <memory>
#include <vector>

using namespace std::chrono_literals;
using namespace std::chrono;
using namespace std;

namespace helix {

namespace nasdaq {

nordic_itch_session::nordic_itch_session(shared_ptr<nordic_itch_handler> handler,
                                         shared_ptr<net::message_parser> transport_session,
                                         void *data)
    : session{data}
    , _handler{std::move(handler)}
    , _transport_session{std::move(transport_session)}
{
}

bool nordic_itch_session::is_rth_timestamp(uint64_t timestamp)
{
    // FIXME: This is valid only for Stockholm and Helsinki equities.
    constexpr uint64_t rth_start = duration_cast<milliseconds>(9h).count();
    constexpr uint64_t rth_end   = duration_cast<milliseconds>(17h + 25min).count();
    return timestamp >= rth_start && timestamp < rth_end;
}

void nordic_itch_session::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler->subscribe(symbol, max_orders);
}

size_t nordic_itch_session::process_packet(const net::packet_view& packet)
{
    return _transport_session->parse(packet);
}

void nordic_itch_session::register_callback(core::event_callback callback)
{
   _handler->register_callback(callback);
}

nordic_itch_protocol::nordic_itch_protocol(std::string name)
    : _name{std::move(name)}
{
}

bool nordic_itch_protocol::supports(const std::string& name)
{
    return name == "nasdaq-nordic-moldudp-itch"
        || name == "nasdaq-nordic-soupfile-itch";
}

nordic_itch_session* nordic_itch_protocol::new_session(void *data)
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
