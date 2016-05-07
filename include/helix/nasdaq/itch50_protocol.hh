#pragma once

#include "helix/helix.hh"

#include <string>

namespace helix {

namespace nasdaq {

class itch50_protocol : public protocol {
    std::string _name;
public:
    explicit itch50_protocol(std::string name);
    static bool supports(const std::string& name);
    virtual session* new_session(void *) override;
};

}

}
