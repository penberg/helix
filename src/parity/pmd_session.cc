#include "helix/parity/pmd_session.hh"

#include "helix/parity/pmd_handler.hh"
#include "helix/nasdaq/moldudp64.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>

using namespace std;

namespace helix {

namespace parity {

bool pmd_protocol::supports(const std::string& name)
{
    return name == "parity-moldudp64-pmd";
}

core::session*
pmd_protocol::new_session(void *data)
{
    return new nasdaq::moldudp64_session<pmd_handler>(data);
}

}

}
