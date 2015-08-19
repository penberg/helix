#pragma once

#include "helix/helix.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>
#include <string>

namespace helix {

namespace nasdaq {

class nordic_itch_handler;

class nordic_itch_session : public core::session {
private:
    std::shared_ptr<nordic_itch_handler> _handler;
    std::shared_ptr<net::message_parser> _transport_session;
public:
    nordic_itch_session(std::shared_ptr<nordic_itch_handler>&&, std::shared_ptr<net::message_parser>&&, void *data);
    virtual void subscribe(const std::string& symbol, size_t max_orders) override;
    virtual void register_callback(core::ob_callback process_ob) override;
    virtual void register_callback(core::trade_callback process_trade) override;
    virtual size_t process_packet(const net::packet_view& packet) override;
};

class nordic_itch_protocol : public core::protocol {
    std::string _name;
public:
    nordic_itch_protocol(const std::string& name)
        : _name{name}
    { }
    virtual nordic_itch_session* new_session(void *) override;
};

}

}
