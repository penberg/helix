#include "helix/parity/pmd_protocol.hh"

#include "helix/parity/pmd_handler.hh"
#include "helix/nasdaq/moldudp64.hh"

namespace helix {

namespace parity {

bool pmd_protocol::supports(const std::string& name)
{
    return name == "parity-moldudp64-pmd";
}

pmd_protocol::pmd_protocol(std::string name)
    : _name{std::move(name)}
{
}

session* pmd_protocol::new_session(void *data)
{
    return new nasdaq::moldudp64_session<pmd_handler>(data);
}

}

}
