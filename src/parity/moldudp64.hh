/*
 * MoldUDP64 protocol support
 *
 * The implementation is based on the following specifications provided
 * by NASDAQ:
 *
 *   MoldUDP64 Protocol Specification
 *   V 1.00
 *   07/07/2009
 */

#pragma once

#include "helix/net.hh"

#include <cstdint>
#include <memory>

namespace helix {

namespace parity {

class moldudp64_session : public net::message_parser {
private:
    std::shared_ptr<net::message_parser> _parser;
    uint64_t _seq_num;
public:
    explicit moldudp64_session(std::shared_ptr<net::message_parser> parser);

    virtual size_t parse(const net::packet_view& packet) override;
};

}

}
