#pragma once

#include "helix/helix.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>
#include <string>

namespace helix {

namespace nasdaq {

class nordic_itch_handler;

class nordic_itch_protocol : public core::protocol {
    std::string _name;
public:
    static bool supports(const std::string& name);
    explicit nordic_itch_protocol(std::string name);
    virtual core::session* new_session(void *) override;
};

}

}
