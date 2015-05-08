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
class itch_session;

class nordic_itch_session : public core::session {
private:
    std::shared_ptr<itch_session> _itch_session;
    std::shared_ptr<moldudp_session> _moldudp_session;
public:
    nordic_itch_session(const std::vector<std::string>& symbols, void *data);
    virtual void register_callback(core::ob_callback process_ob) override;
    virtual void register_callback(core::trade_callback process_trade) override;
    virtual size_t process_packet(const net::packet_view& packet) override;
};

class nordic_itch_protocol : public core::protocol {
    virtual nordic_itch_session* new_session(const std::vector<std::string>&, void *) override;
};

}

}
