#include "helix/nasdaq/nordic_itch.hh"

#include "itch_session.hh"
#include "moldudp.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace nasdaq {

nordic_itch_session::nordic_itch_session(const vector<string>& symbols, core::callback process)
    : _session{make_shared<moldudp_session>(make_shared<itch_session>(symbols, process))}
{
}

void nordic_itch_session::process_packet(const char* buf, size_t size)
{
    _session->parse(buf, size);
}

nordic_itch_session* nordic_itch_protocol::new_session(const vector<string>& symbols, core::callback process)
{
    return new nordic_itch_session(symbols, process);
}

}

}
