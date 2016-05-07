#pragma once

#include "helix/helix.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>
#include <string>

namespace helix {

namespace parity {

class pmd_protocol : public protocol {
    std::string _name;
public:
    static bool supports(const std::string& name);

    explicit pmd_protocol(std::string name);
    virtual session* new_session(void *) override;
};

}

}
