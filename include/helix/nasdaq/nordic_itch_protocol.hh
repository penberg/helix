#pragma once

#include "helix/helix.hh"

#include <string>

namespace helix {

namespace nasdaq {

class nordic_itch_protocol : public core::protocol {
    std::string _name;
public:
    static bool supports(const std::string& name);
    explicit nordic_itch_protocol(std::string name);
    virtual core::session* new_session(void *) override;
};

}

}
