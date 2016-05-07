#include "helix/nasdaq/nordic_itch_session.hh"

#include "helix/nasdaq/nordic_itch_handler.hh"
#include "helix/net.hh"
#include "helix/nasdaq/soupfile.hh"
#include "helix/nasdaq/moldudp.hh"

#include <memory>
#include <vector>

namespace helix {

namespace nasdaq {

nordic_itch_protocol::nordic_itch_protocol(std::string name)
    : _name{std::move(name)}
{
}

bool nordic_itch_protocol::supports(const std::string& name)
{
    return name == "nasdaq-nordic-moldudp-itch"
        || name == "nasdaq-nordic-soupfile-itch";
}

core::session* nordic_itch_protocol::new_session(void *data)
{
    if (_name == "nasdaq-nordic-moldudp-itch") {
        return new moldudp_session<nordic_itch_handler>(data);
    } else if (_name == "nasdaq-nordic-soupfile-itch") {
        return new soupfile_session<nordic_itch_handler>(data);
    } else {
        throw std::invalid_argument("unknown protocol: " + _name);
    }
}

}

}
