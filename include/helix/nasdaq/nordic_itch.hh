/*
 * Copyright (C) 2015 Bitbot
 */

#pragma once

#include "helix/helix.hh"

#include <memory>
#include <vector>
#include <string>

namespace helix {

namespace nasdaq {

class moldudp_session;

class nordic_itch_session : public core::session {
private:
    std::shared_ptr<moldudp_session> _session;
public:
    nordic_itch_session(const std::vector<std::string>& symbols, core::callback process);

    virtual void process_packet(const char *buf, size_t size) override;
};

class nordic_itch_protocol : public core::protocol {
    virtual nordic_itch_session* new_session(const std::vector<std::string>&, core::callback process) override;
};

}

}
