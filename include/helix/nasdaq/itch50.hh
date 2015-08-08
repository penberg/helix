#pragma once

#include "helix/helix.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>
#include <string>

namespace helix {

namespace nasdaq {

class itch50_session_impl;

class itch50_session : public core::session {
private:
    std::shared_ptr<itch50_session_impl> _itch_session;
    std::shared_ptr<net::message_parser> _transport_session;
public:
    itch50_session(std::shared_ptr<itch50_session_impl>&&, std::shared_ptr<net::message_parser>&&, void *data);
    virtual void register_callback(core::ob_callback process_ob) override;
    virtual void register_callback(core::trade_callback process_trade) override;
    virtual size_t process_packet(const net::packet_view& packet) override;
};

class itch50_protocol : public core::protocol {
    std::string _name;
public:
    itch50_protocol(const std::string& name)
        : _name{name}
    { }
    virtual itch50_session* new_session(const std::vector<std::string>&, void *) override;
};

}

}
