#include "helix/nasdaq/nordic_itch.hh"

#include "itch_session.hh"
#include "helix/net.hh"
#include "moldudp.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace nasdaq {

nordic_itch_session::nordic_itch_session(const vector<string>& symbols, core::ob_callback process_ob, core::trade_callback process_trade, void *data)
    : session{data}
    , _session{make_shared<moldudp_session>(make_shared<itch_session>(symbols, process_ob, process_trade))}
{
}

void nordic_itch_session::process_packet(const net::packet_view& packet)
{
    _session->parse(packet.buf(), packet.len());
}

nordic_itch_session*
nordic_itch_protocol::new_session(const vector<string>& symbols, core::ob_callback process_ob, core::trade_callback process_trade, void *data)
{
    return new nordic_itch_session(symbols, process_ob, process_trade, data);
}

}

}
