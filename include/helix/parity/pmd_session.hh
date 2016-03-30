#pragma once

#include "helix/helix.hh"
#include "helix/net.hh"

#include <memory>
#include <vector>
#include <string>

namespace helix {

namespace parity {

class pmd_handler;

class pmd_session : public core::session {
private:
    std::shared_ptr<pmd_handler> _handler;
    std::shared_ptr<net::message_parser> _transport_session;
public:
    pmd_session(std::shared_ptr<pmd_handler>, std::shared_ptr<net::message_parser>, void *data);
    virtual bool is_rth_timestamp(uint64_t timestamp) override;
    virtual void subscribe(const std::string& symbol, size_t max_orders) override;
    virtual void register_callback(core::event_callback callback) override;
    virtual size_t process_packet(const net::packet_view& packet) override;
};

class pmd_protocol : public core::protocol {
    std::string _name;
public:
    explicit pmd_protocol(std::string name)
        : _name{std::move(name)}
    { }
    static bool supports(const std::string& name);
    virtual pmd_session* new_session(void *) override;
};

}

}
