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
    std::shared_ptr<nordic_itch_handler> _handler;
    std::shared_ptr<net::message_parser> _transport_session;
public:
    nordic_itch_session(std::shared_ptr<nordic_itch_handler>, std::shared_ptr<net::message_parser>, void *data);
    virtual bool is_rth_timestamp(uint64_t timestamp) override;
    virtual void subscribe(const std::string& symbol, size_t max_orders) override;
    virtual void register_callback(core::event_callback callback) override;
    virtual size_t process_packet(const net::packet_view& packet) override;
};

class nordic_itch_protocol : public core::protocol {
    std::string _name;
public:
    explicit nordic_itch_protocol(std::string name);
    virtual nordic_itch_session* new_session(void *) override;
};

}

}
