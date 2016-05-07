#include "helix/nasdaq/itch50_protocol.hh"

#include "helix/nasdaq/itch50_handler.hh"
#include "helix/nasdaq/binaryfile.hh"
#include "helix/net.hh"

#include <chrono>
#include <memory>
#include <vector>

namespace helix {

namespace nasdaq {

bool itch50_protocol::supports(const std::string& name)
{
    return name == "nasdaq-binaryfile-itch50";
}

core::session*
itch50_protocol::new_session(void *data)
{
    if (_name == "nasdaq-binaryfile-itch50") {
        return new binaryfile_session<itch50_handler>(data);
    } else {
        throw std::invalid_argument("unknown protocol: " + _name);
    }
}

}

}
