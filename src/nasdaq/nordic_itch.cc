#include "helix/nasdaq/nordic_itch.hh"

#include "itch_session.hh"
#include "helix/net.hh"
#include "moldudp.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace nasdaq {

nordic_itch_session::nordic_itch_session(const vector<string>& symbols, void *data)
    : session{data}
    , _itch_session{make_shared<itch_session>(symbols)}
    , _moldudp_session{make_shared<moldudp_session>(_itch_session)}
{
}

size_t nordic_itch_session::process_packet(const net::packet_view& packet)
{
    return _moldudp_session->parse(packet);
}

void nordic_itch_session::register_callback(core::ob_callback process_ob)
{
   _itch_session->register_callback(process_ob);
}

void nordic_itch_session::register_callback(core::trade_callback process_trade)
{
   _itch_session->register_callback(process_trade);
}

nordic_itch_session*
nordic_itch_protocol::new_session(const vector<string>& symbols, void *data)
{
    return new nordic_itch_session(symbols, data);
}

}

}
