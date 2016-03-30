#include "helix/parity/pmd_session.hh"

#include "helix/parity/pmd_handler.hh"
#include "helix/net.hh"
#include "moldudp64.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace parity {

pmd_session::pmd_session(shared_ptr<pmd_handler> handler,
                         shared_ptr<net::message_parser> transport_session,
                         void *data)
    : session{data}
    , _handler{std::move(handler)}
    , _transport_session{std::move(transport_session)}
{
}

bool pmd_session::is_rth_timestamp(uint64_t timestamp)
{
    return true;
}

void pmd_session::subscribe(const std::string& symbol, size_t max_orders)
{
    _handler->subscribe(symbol, max_orders);
}

size_t pmd_session::process_packet(const net::packet_view& packet)
{
    return _transport_session->parse(packet);
}

void pmd_session::register_callback(core::event_callback callback)
{
   _handler->register_callback(callback);
}

bool pmd_protocol::supports(const std::string& name)
{
    return name == "parity-moldudp64-pmd";
}

pmd_session*
pmd_protocol::new_session(void *data)
{
    auto is = make_shared<pmd_handler>();
    shared_ptr<net::message_parser> ts = make_shared<moldudp64_session>(is);
    return new pmd_session(std::move(is), std::move(ts), data);
}

}

}
