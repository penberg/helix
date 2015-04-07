#include "helix/nasdaq/nordic_itch.hh"

#include "itch_session.hh"
#include "moldudp.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace nasdaq {

nordic_itch_session::nordic_itch_session(const vector<string>& symbols, core::ob_callback process_ob, core::trade_callback process_trade)
    : _session{make_shared<moldudp_session>(make_shared<itch_session>(symbols, process_ob, process_trade))}
{
}

void nordic_itch_session::process_packet(const char* buf, size_t size)
{
    _session->parse(buf, size);
}

nordic_itch_session*
nordic_itch_protocol::new_session(const vector<string>& symbols, core::ob_callback process_ob, core::trade_callback process_trade)
{
    return new nordic_itch_session(symbols, process_ob, process_trade);
}

}

}
