#include "helix/nasdaq/itch50_session.hh"

#include "helix/nasdaq/itch50_handler.hh"
#include "binaryfile.hh"
#include "helix/net.hh"

#include <chrono>
#include <memory>
#include <vector>

using namespace std::chrono_literals;
using namespace std::chrono;
using namespace std;

namespace helix {

namespace nasdaq {

itch50_session::itch50_session(shared_ptr<itch50_handler> handler,
                             shared_ptr<net::message_parser> transport_session,
                             void *data)
    : session{data}
    , _handler{std::move(handler)}
    , _transport_session{std::move(transport_session)}
{
}

bool itch50_session::is_rth_timestamp(uint64_t timestamp)
{
    constexpr uint64_t rth_start = duration_cast<nanoseconds>(9h + 30min).count();
    constexpr uint64_t rth_end   = duration_cast<nanoseconds>(16h).count();
    return timestamp >= rth_start && timestamp < rth_end;
}

void itch50_session::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler->subscribe(symbol, max_orders);
}

size_t itch50_session::process_packet(const net::packet_view& packet)
{
    return _transport_session->parse(packet);
}

void itch50_session::register_callback(core::event_callback callback)
{
   _handler->register_callback(callback);
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
