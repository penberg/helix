#pragma once

#include "helix/helix.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>
#include <string>

namespace helix {

namespace parity {

class pmd_protocol : public core::protocol {
    std::string _name;
public:
    explicit pmd_protocol(std::string name)
        : _name{std::move(name)}
    { }
    static bool supports(const std::string& name);
    virtual core::session* new_session(void *) override;
};

}

}
